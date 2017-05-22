/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_plot_layout.h"
#include "qwt_text.h"
#include "qwt_text_label.h"
#include "qwt_plot_canvas.h"
#include "qwt_scale_widget.h"
#include "qwt_legend.h"
#include <qscrollbar.h>
#include <qmath.h>

class QwtPlotLayout::LayoutData
{
public:
    void init( const QwtPlot *, const QRectF &rect );

    struct t_legendData
    {
        int vScrollBarWidth;
        int hScrollBarHeight;
        QSize hint;
    } legend;

    struct t_titleData
    {
        QwtText text;
        int frameWidth;
    } title;

    struct t_scaleData
    {
        const QwtScaleWidget *scaleWidget;
        QFont scaleFont;
        int baseLineOffset;
        int tickOffset;
        int dimWithoutTitle;
        bool isEnabled;
    } scale[QwtPlot::axisCnt];

    struct t_canvasData
    {
        int frameWidth;
    } canvas;
};

/*
  Extract all layout relevant data from the plot components
*/

void QwtPlotLayout::LayoutData::init( const QwtPlot *plot, const QRectF &rect )
{
    // legend

    if ( plot->plotLayout()->legendPosition() != QwtPlot::ExternalLegend
        && plot->legend() )
    {
        legend.vScrollBarWidth =
            plot->legend()->verticalScrollBar()->sizeHint().width();
        legend.hScrollBarHeight =
            plot->legend()->horizontalScrollBar()->sizeHint().height();

        const QSize hint = plot->legend()->sizeHint();

        int w = qMin( hint.width(), ( int )rect.width() );
        int h = plot->legend()->heightForWidth( w );
        if ( h == 0 )
            h = hint.height();

        if ( h > rect.height() )
            w += legend.vScrollBarWidth;

        legend.hint = QSize( w, h );
    }

    // title

    title.frameWidth = 0;
    title.text = QwtText();

    if ( plot->titleLabel() )
    {
        const QwtTextLabel *label = plot->titleLabel();
        title.text = label->text();
        if ( !( title.text.testPaintAttribute( QwtText::PaintUsingTextFont ) ) )
            title.text.setFont( label->font() );

        title.frameWidth = plot->titleLabel()->frameWidth();
    }

    // scales

    for ( int axis = 0; axis < QwtPlot::axisCnt; axis++ )
    {
        if ( plot->axisEnabled( axis ) )
        {
            const QwtScaleWidget *scaleWidget = plot->axisWidget( axis );

            scale[axis].isEnabled = true;

            scale[axis].scaleWidget = scaleWidget;

            scale[axis].scaleFont = scaleWidget->font();

            scale[axis].baseLineOffset = scaleWidget->margin();
            scale[axis].tickOffset = scaleWidget->margin();
            scale[axis].tickOffset +=
                ( int )scaleWidget->scaleDraw()->maxTickLength();

            scale[axis].dimWithoutTitle = scaleWidget->dimForLength(
                QWIDGETSIZE_MAX, scale[axis].scaleFont );

            if ( !scaleWidget->title().isEmpty() )
            {
                scale[axis].dimWithoutTitle -=
                    scaleWidget->titleHeightForWidth( QWIDGETSIZE_MAX );
            }
        }
        else
        {
            scale[axis].isEnabled = false;
            scale[axis].baseLineOffset = 0;
            scale[axis].tickOffset = 0;
            scale[axis].dimWithoutTitle = 0;
        }
    }

    // canvas

    canvas.frameWidth = plot->canvas()->frameWidth();
}

class QwtPlotLayout::PrivateData
{
public:
    PrivateData():
        spacing( 5 )
    {
    }

    QRectF titleRect;
    QRectF legendRect;
    QRectF scaleRect[QwtPlot::axisCnt];
    QRectF canvasRect;

    QwtPlotLayout::LayoutData layoutData;

    QwtPlot::LegendPosition legendPos;
    double legendRatio;
    unsigned int spacing;
};

/*!
  \brief Constructor
 */

QwtPlotLayout::QwtPlotLayout()
{
    d_data = new PrivateData;

    setLegendPosition( QwtPlot::BottomLegend );

    invalidate();
}

//! Destructor
QwtPlotLayout::~QwtPlotLayout()
{
    delete d_data;
}

/*!
  Change the spacing of the plot. The spacing is the distance
  between the plot components.

  \param spacing new spacing
  \sa setMargin(), spacing()
*/
void QwtPlotLayout::setSpacing( int spacing )
{
    d_data->spacing = qMax( 0, spacing );
}

/*!
  \return spacing
  \sa margin(), setSpacing()
*/
int QwtPlotLayout::spacing() const
{
    return d_data->spacing;
}

/*!
  \brief Specify the position of the legend
  \param pos The legend's position.
  \param ratio Ratio between legend and the bounding rect
               of title, canvas and axes. The legend will be shrinked
               if it would need more space than the given ratio.
               The ratio is limited to ]0.0 .. 1.0]. In case of <= 0.0
               it will be reset to the default ratio.
               The default vertical/horizontal ratio is 0.33/0.5.

  \sa QwtPlot::setLegendPosition()
*/

void QwtPlotLayout::setLegendPosition( QwtPlot::LegendPosition pos, double ratio )
{
    if ( ratio > 1.0 )
        ratio = 1.0;

    switch ( pos )
    {
        case QwtPlot::TopLegend:
        case QwtPlot::BottomLegend:
            if ( ratio <= 0.0 )
                ratio = 0.33;
            d_data->legendRatio = ratio;
            d_data->legendPos = pos;
            break;
        case QwtPlot::LeftLegend:
        case QwtPlot::RightLegend:
            if ( ratio <= 0.0 )
                ratio = 0.5;
            d_data->legendRatio = ratio;
            d_data->legendPos = pos;
            break;
        case QwtPlot::ExternalLegend:
            d_data->legendRatio = ratio; // meaningless
            d_data->legendPos = pos;
        default:
            break;
    }
}

/*!
  \brief Specify the position of the legend
  \param pos The legend's position. Valid values are
      \c QwtPlot::LeftLegend, \c QwtPlot::RightLegend,
      \c QwtPlot::TopLegend, \c QwtPlot::BottomLegend.

  \sa QwtPlot::setLegendPosition()
*/
void QwtPlotLayout::setLegendPosition( QwtPlot::LegendPosition pos )
{
    setLegendPosition( pos, 0.0 );
}

/*!
  \return Position of the legend
  \sa setLegendPosition(), QwtPlot::setLegendPosition(),
      QwtPlot::legendPosition()
*/
QwtPlot::LegendPosition QwtPlotLayout::legendPosition() const
{
    return d_data->legendPos;
}

/*!
  Specify the relative size of the legend in the plot
  \param ratio Ratio between legend and the bounding rect
               of title, canvas and axes. The legend will be shrinked
               if it would need more space than the given ratio.
               The ratio is limited to ]0.0 .. 1.0]. In case of <= 0.0
               it will be reset to the default ratio.
               The default vertical/horizontal ratio is 0.33/0.5.
*/
void QwtPlotLayout::setLegendRatio( double ratio )
{
    setLegendPosition( legendPosition(), ratio );
}

/*!
  \return The relative size of the legend in the plot.
  \sa setLegendPosition()
*/
double QwtPlotLayout::legendRatio() const
{
    return d_data->legendRatio;
}

/*!
  \return Geometry for the title
  \sa activate(), invalidate()
*/

const QRectF &QwtPlotLayout::titleRect() const
{
    return d_data->titleRect;
}

/*!
  \return Geometry for the legend
  \sa activate(), invalidate()
*/

const QRectF &QwtPlotLayout::legendRect() const
{
    return d_data->legendRect;
}

/*!
  \param axis Axis index
  \return Geometry for the scale
  \sa activate(), invalidate()
*/

const QRectF &QwtPlotLayout::scaleRect( int axis ) const
{
    if ( axis < 0 || axis >= QwtPlot::axisCnt )
    {
        static QRectF dummyRect;
        return dummyRect;
    }
    return d_data->scaleRect[axis];
}

/*!
  \return Geometry for the canvas
  \sa activate(), invalidate()
*/

const QRectF &QwtPlotLayout::canvasRect() const
{
    return d_data->canvasRect;
}

/*!
  Invalidate the geometry of all components.
  \sa activate()
*/
void QwtPlotLayout::invalidate()
{
    d_data->titleRect = d_data->legendRect = d_data->canvasRect = QRect();
    for ( int axis = 0; axis < QwtPlot::axisCnt; axis++ )
        d_data->scaleRect[axis] = QRect();
}

/*!
  \brief Return a minimum size hint
  \sa QwtPlot::minimumSizeHint()
*/

QSize QwtPlotLayout::minimumSizeHint( const QwtPlot *plot ) const
{
    class ScaleData
    {
    public:
        ScaleData()
        {
            w = h = tickOffset = 0;
        }

        int w;
        int h;
        int tickOffset;
    } scaleData[QwtPlot::axisCnt];

    int axis;
    for ( axis = 0; axis < QwtPlot::axisCnt; axis++ )
    {
        if ( plot->axisEnabled( axis ) )
        {
            const QwtScaleWidget *scl = plot->axisWidget( axis );
            ScaleData &sd = scaleData[axis];

            const QSize hint = scl->minimumSizeHint();
            sd.w = hint.width();
            sd.h = hint.height();
            sd.tickOffset = scl->margin();
            sd.tickOffset += scl->scaleDraw()->maxTickLength();
        }
    }

    const QwtPlotCanvas *canvas = plot->canvas();
    const QSize minCanvasSize = canvas->minimumSize();

    int w = scaleData[QwtPlot::yLeft].w + scaleData[QwtPlot::yRight].w;
    int cw = qMax( scaleData[QwtPlot::xBottom].w, scaleData[QwtPlot::xTop].w )
        + 2 * ( canvas->frameWidth() + 1 );
    w += qMax( cw, minCanvasSize.width() );

    int h = scaleData[QwtPlot::xBottom].h + scaleData[QwtPlot::xTop].h;
    int ch = qMax( scaleData[QwtPlot::yLeft].h, scaleData[QwtPlot::yRight].h )
        + 2 * ( canvas->frameWidth() + 1 );
    h += qMax( ch, minCanvasSize.height() );

    const QwtTextLabel *title = plot->titleLabel();
    if ( title && !title->text().isEmpty() )
    {
        // If only QwtPlot::yLeft or QwtPlot::yRight is showing,
        // we center on the plot canvas.
        const bool centerOnCanvas = !( plot->axisEnabled( QwtPlot::yLeft )
            && plot->axisEnabled( QwtPlot::yRight ) );

        int titleW = w;
        if ( centerOnCanvas )
        {
            titleW -= scaleData[QwtPlot::yLeft].w
                + scaleData[QwtPlot::yRight].w;
        }

        int titleH = title->heightForWidth( titleW );
        if ( titleH > titleW ) // Compensate for a long title
        {
            w = titleW = titleH;
            if ( centerOnCanvas )
            {
                w += scaleData[QwtPlot::yLeft].w
                    + scaleData[QwtPlot::yRight].w;
            }

            titleH = title->heightForWidth( titleW );
        }
        h += titleH + d_data->spacing;
    }

    // Compute the legend contribution

    const QwtLegend *legend = plot->legend();
    if ( d_data->legendPos != QwtPlot::ExternalLegend
        && legend && !legend->isEmpty() )
    {
        if ( d_data->legendPos == QwtPlot::LeftLegend
            || d_data->legendPos == QwtPlot::RightLegend )
        {
            int legendW = legend->sizeHint().width();
            int legendH = legend->heightForWidth( legendW );

            if ( legend->frameWidth() > 0 )
                w += d_data->spacing;

            if ( legendH > h )
                legendW += legend->verticalScrollBar()->sizeHint().height();

            if ( d_data->legendRatio < 1.0 )
                legendW = qMin( legendW, int( w / ( 1.0 - d_data->legendRatio ) ) );

            w += legendW + d_data->spacing;
        }
        else // QwtPlot::Top, QwtPlot::Bottom
        {
            int legendW = qMin( legend->sizeHint().width(), w );
            int legendH = legend->heightForWidth( legendW );

            if ( legend->frameWidth() > 0 )
                h += d_data->spacing;

            if ( d_data->legendRatio < 1.0 )
                legendH = qMin( legendH, int( h / ( 1.0 - d_data->legendRatio ) ) );

            h += legendH + d_data->spacing;
        }
    }

    return QSize( w, h );
}

/*!
  Find the geometry for the legend
  \param rect Rectangle where to place the legend
  \return Geometry for the legend
*/

QRectF QwtPlotLayout::layoutLegend( const QRectF &rect ) const
{
    const QSize hint( d_data->layoutData.legend.hint );

    int dim;
    if ( d_data->legendPos == QwtPlot::LeftLegend
        || d_data->legendPos == QwtPlot::RightLegend )
    {
        // We don't allow vertical legends to take more than
        // half of the available space.

        dim = qMin( hint.width(), int( rect.width() * d_data->legendRatio ) );

        if ( hint.height() > rect.height() )
        {
            // The legend will need additional
            // space for the vertical scrollbar.

            dim += d_data->layoutData.legend.vScrollBarWidth;
        }
    }
    else
    {
        dim = qMin( hint.height(), int( rect.height() * d_data->legendRatio ) );
        dim = qMax( dim, d_data->layoutData.legend.hScrollBarHeight );
    }

    QRectF legendRect = rect;
    switch ( d_data->legendPos )
    {
        case QwtPlot::LeftLegend:
            legendRect.setWidth( dim );
            break;
        case QwtPlot::RightLegend:
            legendRect.setX( rect.right() - dim );
            legendRect.setWidth( dim );
            break;
        case QwtPlot::TopLegend:
            legendRect.setHeight( dim );
            break;
        case QwtPlot::BottomLegend:
            legendRect.setY( rect.bottom() - dim );
            legendRect.setHeight( dim );
            break;
        case QwtPlot::ExternalLegend:
            break;
    }

    return legendRect;
}

/*!
  Align the legend to the canvas
  \param canvasRect Geometry of the canvas
  \param legendRect Maximum geometry for the legend
  \return Geometry for the aligned legend
*/
QRectF QwtPlotLayout::alignLegend( const QRectF &canvasRect,
    const QRectF &legendRect ) const
{
    QRectF alignedRect = legendRect;

    if ( d_data->legendPos == QwtPlot::BottomLegend
        || d_data->legendPos == QwtPlot::TopLegend )
    {
        if ( d_data->layoutData.legend.hint.width() < canvasRect.width() )
        {
            alignedRect.setX( canvasRect.x() );
            alignedRect.setWidth( canvasRect.width() );
        }
    }
    else
    {
        if ( d_data->layoutData.legend.hint.height() < canvasRect.height() )
        {
            alignedRect.setY( canvasRect.y() );
            alignedRect.setHeight( canvasRect.height() );
        }
    }

    return alignedRect;
}

/*!
  Expand all line breaks in text labels, and calculate the height
  of their widgets in orientation of the text.

  \param rect Bounding rect for title, axes and canvas.
  \param dimTitle Expanded height of the title widget
  \param dimAxis Expanded heights of the axis in axis orientation.
*/
void QwtPlotLayout::expandLineBreaks( const QRectF &rect,
    int &dimTitle, int dimAxis[QwtPlot::axisCnt] ) const
{
    dimTitle = 0;
    for ( int axis = 0; axis < QwtPlot::axisCnt; axis++ )
        dimAxis[axis] = 0;

    int backboneOffset = d_data->layoutData.canvas.frameWidth;

    bool done = false;
    while ( !done )
    {
        done = true;

        // the size for the 4 axis depend on each other. Expanding
        // the height of a horizontal axis will shrink the height
        // for the vertical axis, shrinking the height of a vertical
        // axis will result in a line break what will expand the
        // width and results in shrinking the width of a horizontal
        // axis what might result in a line break of a horizontal
        // axis ... . So we loop as long until no size changes.

        if ( !d_data->layoutData.title.text.isEmpty() )
        {
            int w = rect.width();

            if ( d_data->layoutData.scale[QwtPlot::yLeft].isEnabled
                != d_data->layoutData.scale[QwtPlot::yRight].isEnabled )
            {
                // center to the canvas
                w -= dimAxis[QwtPlot::yLeft] + dimAxis[QwtPlot::yRight];
            }

            int d = qCeil( d_data->layoutData.title.text.heightForWidth( w ) );
            d += 2 * d_data->layoutData.title.frameWidth;

            if ( d > dimTitle )
            {
                dimTitle = d;
                done = false;
            }
        }

        for ( int axis = 0; axis < QwtPlot::axisCnt; axis++ )
        {
            const struct LayoutData::t_scaleData &scaleData =
                d_data->layoutData.scale[axis];

            if ( scaleData.isEnabled )
            {
                int length;
                if ( axis == QwtPlot::xTop || axis == QwtPlot::xBottom )
                {
                    length = rect.width() - dimAxis[QwtPlot::yLeft]
                        - dimAxis[QwtPlot::yRight];

                    if ( dimAxis[QwtPlot::yRight] > 0 )
                        length -= 1;

                    length += qMin( dimAxis[QwtPlot::yLeft], -backboneOffset );
                    length += qMin( dimAxis[QwtPlot::yRight], -backboneOffset );
                }
                else // QwtPlot::yLeft, QwtPlot::yRight
                {
                    length = rect.height() - dimAxis[QwtPlot::xTop]
                        - dimAxis[QwtPlot::xBottom];
                    length -= 1;

                    if ( dimAxis[QwtPlot::xBottom] <= 0 )
                        length -= 1;
                    if ( dimAxis[QwtPlot::xTop] <= 0 )
                        length -= 1;

                    if ( dimAxis[QwtPlot::xBottom] > 0 )
                    {
                        length += qMin(
                            d_data->layoutData.scale[QwtPlot::xBottom].tickOffset, -backboneOffset );
                    }
                    if ( dimAxis[QwtPlot::xTop] > 0 )
                    {
                        length += qMin(
                            d_data->layoutData.scale[QwtPlot::xTop].tickOffset, -backboneOffset );
                    }

                    if ( dimTitle > 0 )
                        length -= dimTitle + d_data->spacing;
                }

                int d = scaleData.dimWithoutTitle;
                if ( !scaleData.scaleWidget->title().isEmpty() )
                {
                    d += scaleData.scaleWidget->titleHeightForWidth( length );
                }


                if ( d > dimAxis[axis] )
                {
                    dimAxis[axis] = d;
                    done = false;
                }
            }
        }
    }
}

/*!
  \brief Recalculate the geometry of all components.

  \param plot Plot to be layout
  \param plotRect Rect where to place the components

  \sa invalidate(), titleRect(),
      legendRect(), scaleRect(), canvasRect()
*/
void QwtPlotLayout::activate( const QwtPlot *plot,
    const QRectF &plotRect )
{
    invalidate();

    QRectF rect( plotRect );  // undistributed rest of the plot rect

    // We extract all layout relevant data from the widgets,
    // filter them through pfilter and save them to d_data->layoutData.

    d_data->layoutData.init( plot, rect );

    if ( d_data->legendPos != QwtPlot::ExternalLegend
        && plot->legend() && !plot->legend()->isEmpty() )
    {
        d_data->legendRect = layoutLegend( rect );

        // subtract d_data->legendRect from rect

        const QRegion region( rect.toRect() );
        rect = region.subtract( d_data->legendRect.toRect() ).boundingRect();

        switch ( d_data->legendPos )
        {
            case QwtPlot::LeftLegend:
                rect.setLeft( rect.left() + d_data->spacing );
                break;
            case QwtPlot::RightLegend:
                rect.setRight( rect.right() - d_data->spacing );
                break;
            case QwtPlot::TopLegend:
                rect.setTop( rect.top() + d_data->spacing );
                break;
            case QwtPlot::BottomLegend:
                rect.setBottom( rect.bottom() - d_data->spacing );
                break;
            case QwtPlot::ExternalLegend:
                break; // suppress compiler warning
        }
    }

    /*
     +---+-----------+---+
     |       Title       |
     +---+-----------+---+
     |   |   Axis    |   |
     +---+-----------+---+
     | A |           | A |
     | x |  Canvas   | x |
     | i |           | i |
     | s |           | s |
     +---+-----------+---+
     |   |   Axis    |   |
     +---+-----------+---+
    */

    // axes and title include text labels. The height of each
    // label depends on its line breaks, that depend on the width
    // for the label. A line break in a horizontal text will reduce
    // the available width for vertical texts and vice versa.
    // expandLineBreaks finds the height/width for title and axes
    // including all line breaks.

    int dimTitle, dimAxes[QwtPlot::axisCnt];
    expandLineBreaks( rect, dimTitle, dimAxes );

    if ( dimTitle > 0 )
    {
        d_data->titleRect = QRect( rect.x(), rect.y(),
            rect.width(), dimTitle );

        if ( d_data->layoutData.scale[QwtPlot::yLeft].isEnabled !=
            d_data->layoutData.scale[QwtPlot::yRight].isEnabled )
        {
            // if only one of the y axes is missing we align
            // the title centered to the canvas

            d_data->titleRect.setX( rect.x() + dimAxes[QwtPlot::yLeft] );
            d_data->titleRect.setWidth( rect.width()
                - dimAxes[QwtPlot::yLeft] - dimAxes[QwtPlot::yRight] );
        }

        // subtract title
        rect.setTop( rect.top() + dimTitle + d_data->spacing );
    }

    d_data->canvasRect.setRect(
        rect.x() + dimAxes[QwtPlot::yLeft],
        rect.y() + dimAxes[QwtPlot::xTop],
        rect.width() - dimAxes[QwtPlot::yRight] - dimAxes[QwtPlot::yLeft],
        rect.height() - dimAxes[QwtPlot::xBottom] - dimAxes[QwtPlot::xTop] );

    int fw = d_data->layoutData.canvas.frameWidth;
    for ( int axis = 0; axis < QwtPlot::axisCnt; axis++ )
    {
        // set the rects for the axes

        if ( dimAxes[axis] )
        {
            int dim = dimAxes[axis];
            QRectF &scaleRect = d_data->scaleRect[axis];

            switch ( axis )
            {
                case QwtPlot::yLeft:
                    scaleRect.setX( d_data->canvasRect.left() - dim );
                    scaleRect.setWidth( dim );
                    break;
                case QwtPlot::yRight:
                    scaleRect.setX( d_data->canvasRect.right() );
                    scaleRect.setWidth( dim );
                    break;
                case QwtPlot::xBottom:
                    scaleRect.setY( d_data->canvasRect.bottom() );
                    scaleRect.setHeight( dim );
                    break;
                case QwtPlot::xTop:
                    scaleRect.setY( d_data->canvasRect.top() - dim );
                    scaleRect.setHeight( dim );
                    break;
            }

            if ( axis == QwtPlot::xTop || axis == QwtPlot::xBottom )
            {
                scaleRect.setLeft( d_data->canvasRect.left() + fw );
                scaleRect.setRight( d_data->canvasRect.right() - fw - 1 );
            }
            else // QwtPlot::yLeft, QwtPlot::yRight
            {
                scaleRect.setTop( d_data->canvasRect.top() + fw );
                scaleRect.setBottom( d_data->canvasRect.bottom() - fw - 1 );
            }
        }
    }

    if ( !d_data->legendRect.isEmpty() )
    {
        // We prefer to align the legend to the canvas - not to
        // the complete plot - if possible.

        d_data->legendRect = alignLegend( d_data->canvasRect, d_data->legendRect );
    }
}
