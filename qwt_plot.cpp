/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_plot.h"
#include "qwt_plot_dict.h"
#include "qwt_plot_layout.h"
#include "qwt_scale_widget.h"
#include "qwt_scale_engine.h"
#include "qwt_text_label.h"
#include "qwt_legend.h"
#include "qwt_dyngrid_layout.h"
#include "qwt_plot_canvas.h"
#include <qpainter.h>
#include <qpointer.h>
#include <qpaintengine.h>
#include <qapplication.h>
#include <qevent.h>

class QwtPlot::PrivateData
{
public:
    QPointer<QwtTextLabel> lblTitle;
    QPointer<QwtPlotCanvas> canvas;
    QPointer<QwtLegend> legend;
    QwtPlotLayout *layout;
};

/*!
  \brief Constructor
  \param parent Parent widget
 */
QwtPlot::QwtPlot( QWidget *parent ):
    QFrame( parent )
{
    initPlot( QwtText() );
}

/*!
  \brief Constructor
  \param title Title text
  \param parent Parent widget
 */
QwtPlot::QwtPlot( const QwtText &title, QWidget *parent ):
    QFrame( parent )
{
    initPlot( title );
}

//! Destructor
QwtPlot::~QwtPlot()
{
    detachItems( autoDelete() );

    delete d_data->layout;
    deleteAxesData();
    delete d_data;
}

/*!
  \brief Initializes a QwtPlot instance
  \param title Title text
 */
void QwtPlot::initPlot( const QwtText &title )
{
    d_data = new PrivateData;

    d_data->layout = new QwtPlotLayout;

    d_data->lblTitle = new QwtTextLabel( title, this );
    d_data->lblTitle->setObjectName( "QwtPlotTitle" );

    d_data->lblTitle->setFont( QFont( fontInfo().family(), 14, QFont::Bold ) );

    QwtText text( title );
    text.setRenderFlags( Qt::AlignCenter | Qt::TextWordWrap );
    d_data->lblTitle->setText( text );

    d_data->legend = NULL;

    initAxesData();

    d_data->canvas = new QwtPlotCanvas( this );
    d_data->canvas->setObjectName( "QwtPlotCanvas" );
    d_data->canvas->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    d_data->canvas->setLineWidth( 2 );

    updateTabOrder();

    setSizePolicy( QSizePolicy::MinimumExpanding,
        QSizePolicy::MinimumExpanding );

    resize( 200, 200 );
}

/*!
  \brief Adds handling of layout requests
  \param event Event
*/
bool QwtPlot::event( QEvent *event )
{
    bool ok = QFrame::event( event );
    switch ( event->type() )
    {
        case QEvent::LayoutRequest:
            updateLayout();
            break;
        case QEvent::PolishRequest:
            replot();
            break;
        default:;
    }
    return ok;
}

/*!
  Change the plot's title
  \param title New title
*/
void QwtPlot::setTitle( const QString &title )
{
    if ( title != d_data->lblTitle->text().text() )
    {
        d_data->lblTitle->setText( title );
        updateLayout();
    }
}

/*!
  Change the plot's title
  \param title New title
*/
void QwtPlot::setTitle( const QwtText &title )
{
    if ( title != d_data->lblTitle->text() )
    {
        d_data->lblTitle->setText( title );
        updateLayout();
    }
}

//! \return the plot's title
QwtText QwtPlot::title() const
{
    return d_data->lblTitle->text();
}

//! \return the plot's title
QwtPlotLayout *QwtPlot::plotLayout()
{
    return d_data->layout;
}

//! \return the plot's titel label.
const QwtPlotLayout *QwtPlot::plotLayout() const
{
    return d_data->layout;
}

//! \return the plot's titel label.
QwtTextLabel *QwtPlot::titleLabel()
{
    return d_data->lblTitle;
}

/*!
  \return the plot's titel label.
*/
const QwtTextLabel *QwtPlot::titleLabel() const
{
    return d_data->lblTitle;
}

/*!
  \return the plot's legend
  \sa insertLegend()
*/
QwtLegend *QwtPlot::legend()
{
    return d_data->legend;
}

/*!
  \return the plot's legend
  \sa insertLegend()
*/
const QwtLegend *QwtPlot::legend() const
{
    return d_data->legend;
}


/*!
  \return the plot's canvas
*/
QwtPlotCanvas *QwtPlot::canvas()
{
    return d_data->canvas;
}

/*!
  \return the plot's canvas
*/
const QwtPlotCanvas *QwtPlot::canvas() const
{
    return d_data->canvas;
}

/*!
  Return sizeHint
  \sa minimumSizeHint()
*/

QSize QwtPlot::sizeHint() const
{
    int dw = 0;
    int dh = 0;
    for ( int axisId = 0; axisId < axisCnt; axisId++ )
    {
        if ( axisEnabled( axisId ) )
        {
            const int niceDist = 40;
            const QwtScaleWidget *scaleWidget = axisWidget( axisId );
            const QwtScaleDiv &scaleDiv = scaleWidget->scaleDraw()->scaleDiv();
            const int majCnt = scaleDiv.ticks( QwtScaleDiv::MajorTick ).count();

            if ( axisId == yLeft || axisId == yRight )
            {
                int hDiff = ( majCnt - 1 ) * niceDist
                    - scaleWidget->minimumSizeHint().height();
                if ( hDiff > dh )
                    dh = hDiff;
            }
            else
            {
                int wDiff = ( majCnt - 1 ) * niceDist
                    - scaleWidget->minimumSizeHint().width();
                if ( wDiff > dw )
                    dw = wDiff;
            }
        }
    }
    return minimumSizeHint() + QSize( dw, dh );
}

/*!
  \brief Return a minimum size hint
*/
QSize QwtPlot::minimumSizeHint() const
{
    QSize hint = d_data->layout->minimumSizeHint( this );
    hint += QSize( 2 * frameWidth(), 2 * frameWidth() );

    return hint;
}

/*!
  Resize and update internal layout
  \param e Resize event
*/
void QwtPlot::resizeEvent( QResizeEvent *e )
{
    QFrame::resizeEvent( e );
    updateLayout();
}

/*!
  \brief Redraw the plot
*/
void QwtPlot::replot()
{
    updateAxes();

    /*
      Maybe the layout needs to be updated, because of changed
      axes labels. We need to process them here before painting
      to avoid that scales and canvas get out of sync.
     */
    QApplication::sendPostedEvents( this, QEvent::LayoutRequest );

    d_data->canvas->update();
}

/*!
  \brief Adjust plot content to its current size.
  \sa resizeEvent()
*/
void QwtPlot::updateLayout()
{
    d_data->layout->activate( this, contentsRect() );

    QRect titleRect = d_data->layout->titleRect().toRect();
    QRect scaleRect[QwtPlot::axisCnt];
    for ( int axisId = 0; axisId < axisCnt; axisId++ )
        scaleRect[axisId] = d_data->layout->scaleRect( axisId ).toRect();
    QRect legendRect = d_data->layout->legendRect().toRect();
    QRect canvasRect = d_data->layout->canvasRect().toRect();

    //
    // resize and show the visible widgets
    //
    if ( !d_data->lblTitle->text().isEmpty() )
    {
        d_data->lblTitle->setGeometry( titleRect );
        if ( !d_data->lblTitle->isVisibleTo( this ) )
            d_data->lblTitle->show();
    }
    else
        d_data->lblTitle->hide();

    for ( int axisId = 0; axisId < axisCnt; axisId++ )
    {
        if ( axisEnabled( axisId ) )
        {
            if ( axisId == xBottom || axisId == xTop )
                scaleRect[axisId].adjust(0, 0, 1, 0); // +1px padding for last tick
            else
                scaleRect[axisId].adjust(0, 0, 0, 1); // +1px padding for last tick

            axisWidget( axisId )->setGeometry( scaleRect[axisId] );

            if ( !axisWidget( axisId )->isVisibleTo( this ) )
                axisWidget( axisId )->show();
        }
        else
            axisWidget( axisId )->hide();
    }

    if ( d_data->legend &&
        d_data->layout->legendPosition() != ExternalLegend )
    {
        if ( d_data->legend->itemCount() > 0 )
        {
            d_data->legend->setGeometry( legendRect );
            d_data->legend->show();
        }
        else
            d_data->legend->hide();
    }

    d_data->canvas->setGeometry( canvasRect );
}

/*!
   Update the focus tab order

   The order is changed so that the canvas will be in front of the
   first legend item, or behind the last legend item - depending
   on the position of the legend.
*/

void QwtPlot::updateTabOrder()
{
    if ( d_data->canvas->focusPolicy() == Qt::NoFocus )
        return;
    if ( d_data->legend.isNull()
        || d_data->layout->legendPosition() == ExternalLegend
        || d_data->legend->legendItems().count() == 0 )
    {
        return;
    }

    // Depending on the position of the legend the
    // tab order will be changed that the canvas is
    // next to the last legend item, or before
    // the first one.

    const bool canvasFirst =
        d_data->layout->legendPosition() == QwtPlot::BottomLegend ||
        d_data->layout->legendPosition() == QwtPlot::RightLegend;

    QWidget *previous = NULL;

    QWidget *w = d_data->canvas;
    while ( ( w = w->nextInFocusChain() ) != d_data->canvas )
    {
        bool isLegendItem = false;
        if ( w->focusPolicy() != Qt::NoFocus
            && w->parent() && w->parent() == d_data->legend->contentsWidget() )
        {
            isLegendItem = true;
        }

        if ( canvasFirst )
        {
            if ( isLegendItem )
                break;

            previous = w;
        }
        else
        {
            if ( isLegendItem )
                previous = w;
            else
            {
                if ( previous )
                    break;
            }
        }
    }

    if ( previous && previous != d_data->canvas )
        setTabOrder( previous, d_data->canvas );
}

/*!
  Redraw the canvas.
  \param painter Painter used for drawing

  \warning drawCanvas calls drawItems what is also used
           for printing. Applications that like to add individual
           plot items better overload drawItems()
  \sa drawItems()
*/
void QwtPlot::drawCanvas( QPainter *painter )
{
    QwtScaleMap maps[axisCnt];
    for ( int axisId = 0; axisId < axisCnt; axisId++ )
        maps[axisId] = canvasMap( axisId );

    drawItems( painter, d_data->canvas->contentsRect(), maps );
}

/*!
  Redraw the canvas items.
  \param painter Painter used for drawing
  \param canvasRect Bounding rectangle where to paint
  \param map QwtPlot::axisCnt maps, mapping between plot and paint device coordinates
*/

void QwtPlot::drawItems( QPainter *painter, const QRectF &canvasRect,
        const QwtScaleMap map[axisCnt] ) const
{
    const QwtPlotItemList& itmList = itemList();
    for ( QwtPlotItemIterator it = itmList.begin();
        it != itmList.end(); ++it )
    {
        QwtPlotItem *item = *it;
        if ( item && item->isVisible() )
        {
            painter->save();

            painter->setRenderHint( QPainter::Antialiasing,
                item->testRenderHint( QwtPlotItem::RenderAntialiased ) );

            item->draw( painter,
                map[QwtPlot::xBottom], map[QwtPlot::yLeft],
                canvasRect );

            painter->restore();
        }
    }
}

/*!
  \param axisId Axis
  \return Map for the axis on the canvas. With this map pixel coordinates can
          translated to plot coordinates and vice versa.
  \sa QwtScaleMap, transform(), invTransform()

*/
QwtScaleMap QwtPlot::canvasMap( int axisId ) const
{
    QwtScaleMap map;
    if ( !d_data->canvas )
        return map;

    map.setTransformation( axisScaleEngine( axisId )->transformation() );

    const QwtScaleDiv *sd = axisScaleDiv( axisId );
    map.setScaleInterval( sd->lowerBound(), sd->upperBound() );

    if ( axisEnabled( axisId ) )
    {
        const QwtScaleWidget *s = axisWidget( axisId );
        if ( axisId == yLeft || axisId == yRight )
        {
            double y = s->y() - d_data->canvas->y();
            double h = s->height();
            map.setPaintInterval( y + h, y );
        }
        else
        {
            double x = s->x() - d_data->canvas->x();
            double w = s->width();
            map.setPaintInterval( x, x + w );
        }
    }
    else
    {
        const QRect &canvasRect = d_data->canvas->contentsRect();
        if ( axisId == yLeft || axisId == yRight )
        {
            map.setPaintInterval( canvasRect.bottom(),
                canvasRect.top() );
        }
        else
        {
            map.setPaintInterval( canvasRect.left(),
                canvasRect.right() );
        }
    }
    return map;
}

/*!
  \brief Change the background of the plotting area

  Sets brush to QPalette::Window of all colorgroups of
  the palette of the canvas. Using canvas()->setPalette()
  is a more powerful way to set these colors.

  \param brush New background brush
  \sa canvasBackground()
*/
void QwtPlot::setCanvasBackground( const QBrush &brush )
{
    QPalette pal = d_data->canvas->palette();

    for ( int i = 0; i < QPalette::NColorGroups; i++ )
        pal.setBrush( ( QPalette::ColorGroup )i, QPalette::Window, brush );

    canvas()->setPalette( pal );
}

/*!
  Nothing else than: canvas()->palette().brush(
        QPalette::Normal, QPalette::Window);

  \return Background brush of the plotting area.
  \sa setCanvasBackground()
*/
QBrush QwtPlot::canvasBackground() const
{
    return canvas()->palette().brush(
        QPalette::Normal, QPalette::Window );
}

/*!
  \brief Change the border width of the plotting area

  Nothing else than canvas()->setLineWidth(w),
  left for compatibility only.

  \param width New border width
*/
void QwtPlot::setCanvasLineWidth( int width )
{
    canvas()->setLineWidth( width );
    updateLayout();
}

/*!
  Nothing else than: canvas()->lineWidth(),
  left for compatibility only.

  \return the border width of the plotting area
*/
int QwtPlot::canvasLineWidth() const
{
    return canvas()->lineWidth();
}

/*!
  \return \c true if the specified axis exists, otherwise \c false
  \param axisId axis index
 */
bool QwtPlot::axisValid( int axisId )
{
    return ( ( axisId >= QwtPlot::yLeft ) && ( axisId < QwtPlot::axisCnt ) );
}

/*!
  \brief Insert a legend

  If the position legend is \c QwtPlot::LeftLegend or \c QwtPlot::RightLegend
  the legend will be organized in one column from top to down.
  Otherwise the legend items will be placed in a table
  with a best fit number of columns from left to right.

  If pos != QwtPlot::ExternalLegend the plot widget will become
  parent of the legend. It will be deleted when the plot is deleted,
  or another legend is set with insertLegend().

  \param legend Legend
  \param pos The legend's position. For top/left position the number
             of colums will be limited to 1, otherwise it will be set to
             unlimited.

  \param ratio Ratio between legend and the bounding rect
               of title, canvas and axes. The legend will be shrinked
               if it would need more space than the given ratio.
               The ratio is limited to ]0.0 .. 1.0]. In case of <= 0.0
               it will be reset to the default ratio.
               The default vertical/horizontal ratio is 0.33/0.5.

  \sa legend(), QwtPlotLayout::legendPosition(),
      QwtPlotLayout::setLegendPosition()
*/
void QwtPlot::insertLegend( QwtLegend *legend,
    QwtPlot::LegendPosition pos, double ratio )
{
    d_data->layout->setLegendPosition( pos, ratio );

    if ( legend != d_data->legend )
    {
        if ( d_data->legend && d_data->legend->parent() == this )
            delete d_data->legend;

        d_data->legend = legend;

        if ( d_data->legend )
        {
            if ( pos != ExternalLegend )
            {
                if ( d_data->legend->parent() != this )
                    d_data->legend->setParent( this );
            }

            const QwtPlotItemList& itmList = itemList();
            for ( QwtPlotItemIterator it = itmList.begin();
                it != itmList.end(); ++it )
            {
                ( *it )->updateLegend( d_data->legend );
            }

            QLayout *l = d_data->legend->contentsWidget()->layout();
            if ( l && l->inherits( "QwtDynGridLayout" ) )
            {
                QwtDynGridLayout *tl = ( QwtDynGridLayout * )l;
                switch ( d_data->layout->legendPosition() )
                {
                    case LeftLegend:
                    case RightLegend:
                        tl->setMaxCols( 1 ); // 1 column: align vertical
                        break;
                    case TopLegend:
                    case BottomLegend:
                        tl->setMaxCols( 0 ); // unlimited
                        break;
                    case ExternalLegend:
                        break;
                }
            }
        }
        updateTabOrder();
    }

    updateLayout();
}
