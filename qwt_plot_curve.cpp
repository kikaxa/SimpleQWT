/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_plot_curve.h"
#include "qwt_legend.h"
#include "qwt_legend_item.h"
#include "qwt_scale_map.h"
#include "qwt_plot.h"
#include "qwt_plot_canvas.h"
#include "qwt_symbol.h"
#include <qpainter.h>
#include <qpixmap.h>
#include <qalgorithms.h>
#include <qmath.h>

static int verifyRange( int size, int &i1, int &i2 )
{
    if ( size < 1 )
        return 0;

    i1 = qBound( 0, i1, size - 1 );
    i2 = qBound( 0, i2, size - 1 );

    if ( i1 > i2 )
        qSwap( i1, i2 );

    return ( i2 - i1 + 1 );
}

class QwtPlotCurve::PrivateData
{
public:
    PrivateData():
        style( QwtPlotCurve::Lines ),
        baseline( 0.0 ),
        symbol( NULL ),
        attributes( 0 ),
        legendAttributes( 0 )
    {
        pen = QPen( Qt::black );
    }

    ~PrivateData()
    {
        delete symbol;
    }

    QwtPlotCurve::CurveStyle style;
    double baseline;

    const QwtSymbol *symbol;

    QPen pen;
    QBrush brush;

    QwtPlotCurve::CurveAttributes attributes;
    QwtPlotCurve::LegendAttributes legendAttributes;
};

/*!
  Constructor
  \param title Title of the curve
*/
QwtPlotCurve::QwtPlotCurve( const QwtText &title ):
    QwtPlotSeriesItem<QPointF>( title )
{
    init();
}

/*!
  Constructor
  \param title Title of the curve
*/
QwtPlotCurve::QwtPlotCurve( const QString &title ):
    QwtPlotSeriesItem<QPointF>( QwtText( title ) )
{
    init();
}

//! Destructor
QwtPlotCurve::~QwtPlotCurve()
{
    delete d_data;
}

//! Initialize internal members
void QwtPlotCurve::init()
{
    d_data = new PrivateData;
    d_series = new QwtPointSeriesData();

    setZ( 20.0 );
}

/*!
  Specify an attribute how to draw the legend identifier

  \param attribute Attribute
  \param on On/Off
  /sa testLegendAttribute()
*/
void QwtPlotCurve::setLegendAttribute( LegendAttribute attribute, bool on )
{
    if ( on )
        d_data->legendAttributes |= attribute;
    else
        d_data->legendAttributes &= ~attribute;
}

/*!
    \brief Return the current paint attributes
    \sa setLegendAttribute()
*/
bool QwtPlotCurve::testLegendAttribute( LegendAttribute attribute ) const
{
    return ( d_data->legendAttributes & attribute );
}

/*!
  Set the curve's drawing style

  \param style Curve style
  \sa style()
*/
void QwtPlotCurve::setStyle( CurveStyle style )
{
    if ( style != d_data->style )
    {
        d_data->style = style;
        itemChanged();
    }
}

/*!
    Return the current style
    \sa setStyle()
*/
QwtPlotCurve::CurveStyle QwtPlotCurve::style() const
{
    return d_data->style;
}

/*!
  Assign a symbol

  \param symbol Symbol
  \sa symbol()
*/
void QwtPlotCurve::setSymbol( const QwtSymbol *symbol )
{
    if ( symbol != d_data->symbol )
    {
        delete d_data->symbol;
        d_data->symbol = symbol;
        itemChanged();
    }
}

/*!
  \return Current symbol or NULL, when no symbol has been assigned
  \sa setSymbol()
*/
const QwtSymbol *QwtPlotCurve::symbol() const
{
    return d_data->symbol;
}

/*!
  Assign a pen

  \param pen New pen
  \sa pen(), brush()
*/
void QwtPlotCurve::setPen( const QPen &pen )
{
    if ( pen != d_data->pen )
    {
        d_data->pen = pen;
        itemChanged();
    }
}

/*!
  \return Pen used to draw the lines
  \sa setPen(), brush()
*/
const QPen& QwtPlotCurve::pen() const
{
    return d_data->pen;
}

/*!
  \brief Assign a brush.

   In case of brush.style() != QBrush::NoBrush
   and style() != QwtPlotCurve::Sticks
   the area between the curve and the baseline will be filled.

   In case !brush.color().isValid() the area will be filled by
   pen.color(). The fill algorithm simply connects the first and the
   last curve point to the baseline. So the curve data has to be sorted
   (ascending or descending).

  \param brush New brush
  \sa brush(), setBaseline(), baseline()
*/
void QwtPlotCurve::setBrush( const QBrush &brush )
{
    if ( brush != d_data->brush )
    {
        d_data->brush = brush;
        itemChanged();
    }
}

/*!
  \return Brush used to fill the area between lines and the baseline
  \sa setBrush(), setBaseline(), baseline()
*/
const QBrush& QwtPlotCurve::brush() const
{
    return d_data->brush;
}

/*!
  Draw an interval of the curve

  \param painter Painter
  \param xMap Maps x-values into pixel coordinates.
  \param yMap Maps y-values into pixel coordinates.
  \param canvasRect Contents rect of the canvas
  \param from Index of the first point to be painted
  \param to Index of the last point to be painted. If to < 0 the
         curve will be painted to its last point.

  \sa drawCurve(), drawSymbols(),
*/
void QwtPlotCurve::drawSeries( QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap,
    const QRectF &canvasRect, int from, int to ) const
{
    if ( !painter || dataSize() <= 0 )
        return;

    if ( to < 0 )
        to = dataSize() - 1;

    if ( verifyRange( dataSize(), from, to ) > 0 )
    {
        painter->save();
        painter->setPen( d_data->pen );

        /*
          Qt 4.0.0 is slow when drawing lines, but it's even
          slower when the painter has a brush. So we don't
          set the brush before we really need it.
         */

        drawCurve( painter, d_data->style, xMap, yMap, from, to );
        painter->restore();

        if ( d_data->symbol &&
            ( d_data->symbol->style() != QwtSymbol::NoSymbol ) )
        {
            painter->save();
            drawSymbols( painter, *d_data->symbol,
                xMap, yMap, canvasRect, from, to );
            painter->restore();
        }
    }
}

/*!
  \brief Draw the line part (without symbols) of a curve interval.
  \param painter Painter
  \param style curve style, see QwtPlotCurve::CurveStyle
  \param xMap x map
  \param yMap y map
  \param canvasRect Contents rect of the canvas
  \param from index of the first point to be painted
  \param to index of the last point to be painted
  \sa draw(), drawDots(), drawLines(), drawSteps(), drawSticks()
*/
void QwtPlotCurve::drawCurve( QPainter *painter, int style,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap,
    int from, int to ) const
{
    switch ( style )
    {
        case Lines:
            drawLines( painter, xMap, yMap, from, to );
            break;
        case Sticks:
            drawSticks( painter, xMap, yMap, from, to );
            break;
        case Steps:
            drawSteps( painter, xMap, yMap, from, to );
            break;
        case Dots:
            drawDots( painter, xMap, yMap, from, to );
            break;
        case NoCurve:
        default:
            break;
    }
}

/*!
  \brief Draw lines

  If the CurveAttribute Fitted is enabled a QwtCurveFitter tries
  to interpolate/smooth the curve, before it is painted.

  \param painter Painter
  \param xMap x map
  \param yMap y map
  \param canvasRect Contents rect of the canvas
  \param from index of the first point to be painted
  \param to index of the last point to be painted

  \sa setCurveAttribute(), setCurveFitter(), draw(),
      drawLines(), drawDots(), drawSteps(), drawSticks()
*/
#include <qnumeric.h>
void QwtPlotCurve::drawLines( QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap,
    int from, int to ) const
{
    int size = to - from + 1;
    if ( size <= 0 )
        return;

    QPolygonF polyline( size );
    QPointF *points = polyline.data();
    int new_size = 0;

    int prevx = INT_MAX, prevy = INT_MAX;
    double dx = 0, dy = 0; //average distance from pixel center

    for ( int i = from; i <= to; i++ )
    {
        const QPointF sample = d_series->sample( i );

        double x = xMap.transform( sample.x() );
        double y = yMap.transform( sample.y() );
        x = qBound<double>(0, x, INT_MAX);
        y = qBound<double>(0, y, INT_MAX);
        int _x = x;
        int _y = y;
#ifndef QWT_CURVE_NO_SKIP
        if (_x == prevx && _y == prevy)
            continue;
        prevx = _x;
        prevy = _y;
#endif
        dx += (x - _x);
        dy += (y - _y);

        points[new_size].rx() = x;
        points[new_size].ry() = y;
        new_size++;
    }

#ifndef QWT_CURVE_NO_PIXEL_SNAP
    dx /= new_size;
    dy /= new_size;
    double target = painter->pen().widthF() / 2.;
    target = target - int(target); //0.5 for width 1,3..., 0 for 2,4...
    for ( int i = 0; i < new_size; i++ )
    {
        points[i].rx() += (target - dx);
        points[i].ry() += (-target - dy);
    }
#endif

    const int chunkSize = 50;
    for ( int i = 0; i < new_size-1; i += chunkSize-1 )
    {
        const int n = qMin( chunkSize, new_size - i );
        painter->drawPolyline( polyline.constData() + i, n );
    }

    if ( d_data->brush.style() != Qt::NoBrush )
        fillCurve( painter, xMap, yMap, polyline );
}

/*!
  Draw sticks

  \param painter Painter
  \param xMap x map
  \param yMap y map
  \param canvasRect Contents rect of the canvas
  \param from index of the first point to be painted
  \param to index of the last point to be painted

  \sa draw(), drawCurve(), drawDots(), drawLines(), drawSteps()
*/
void QwtPlotCurve::drawSticks( QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap,
    int from, int to ) const
{
    painter->save();
    painter->setRenderHint( QPainter::Antialiasing, false );

    double x0 = xMap.transform( d_data->baseline );
    double y0 = yMap.transform( d_data->baseline );

    const Qt::Orientation o = orientation();

    for ( int i = from; i <= to; i++ )
    {
        const QPointF sample = d_series->sample( i );
        double xi = xMap.transform( sample.x() );
        double yi = yMap.transform( sample.y() );

        if ( o == Qt::Horizontal )
            painter->drawLine( x0, yi, xi, yi );
        else
            painter->drawLine( xi, y0, xi, yi );
    }

    painter->restore();
}

/*!
  Draw dots

  \param painter Painter
  \param xMap x map
  \param yMap y map
  \param canvasRect Contents rect of the canvas
  \param from index of the first point to be painted
  \param to index of the last point to be painted

  \sa draw(), drawCurve(), drawSticks(), drawLines(), drawSteps()
*/
void QwtPlotCurve::drawDots( QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap,
    int from, int to ) const
{
    const bool doFill = d_data->brush.style() != Qt::NoBrush;

    QPolygonF polyline;
    if ( doFill )
        polyline.resize( to - from + 1 );

    QPointF *points = polyline.data();

    for ( int i = from; i <= to; i++ )
    {
        const QPointF sample = d_series->sample( i );
        double xi = xMap.transform( sample.x() );
        double yi = yMap.transform( sample.y() );

        painter->drawPoint( QPointF( xi, yi ) );

        if ( doFill )
        {
            points[i - from].rx() = xi;
            points[i - from].ry() = yi;
        }
    }

    if ( doFill )
        fillCurve( painter, xMap, yMap, polyline );
}

/*!
  Draw step function

  The direction of the steps depends on Inverted attribute.

  \param painter Painter
  \param xMap x map
  \param yMap y map
  \param canvasRect Contents rect of the canvas
  \param from index of the first point to be painted
  \param to index of the last point to be painted

  \sa CurveAttribute, setCurveAttribute(),
      draw(), drawCurve(), drawDots(), drawLines(), drawSticks()
*/
void QwtPlotCurve::drawSteps( QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap,
    int from, int to ) const
{
    QPolygonF polygon( 2 * ( to - from ) + 1 );
    QPointF *points = polygon.data();

    bool inverted = orientation() == Qt::Vertical;
    if ( d_data->attributes & Inverted )
        inverted = !inverted;

    int i, ip;
    for ( i = from, ip = 0; i <= to; i++, ip += 2 )
    {
        const QPointF sample = d_series->sample( i );
        double xi = xMap.transform( sample.x() );
        double yi = yMap.transform( sample.y() );

        if ( ip > 0 )
        {
            const QPointF &p0 = points[ip - 2];
            QPointF &p = points[ip - 1];

            if ( inverted )
            {
                p.rx() = p0.x();
                p.ry() = yi;
            }
            else
            {
                p.rx() = xi;
                p.ry() = p0.y();
            }
        }

        points[ip].rx() = xi;
        points[ip].ry() = yi;
    }

    painter->drawPolyline( polygon.constData(), polygon.size() );

    if ( d_data->brush.style() != Qt::NoBrush )
        fillCurve( painter, xMap, yMap, polygon );
}


/*!
  Specify an attribute for drawing the curve

  \param attribute Curve attribute
  \param on On/Off

  /sa testCurveAttribute(), setCurveFitter()
*/
void QwtPlotCurve::setCurveAttribute( CurveAttribute attribute, bool on )
{
    if ( bool( d_data->attributes & attribute ) == on )
        return;

    if ( on )
        d_data->attributes |= attribute;
    else
        d_data->attributes &= ~attribute;

    itemChanged();
}

/*!
    \return true, if attribute is enabled
    \sa setCurveAttribute()
*/
bool QwtPlotCurve::testCurveAttribute( CurveAttribute attribute ) const
{
    return d_data->attributes & attribute;
}


/*!
  Fill the area between the curve and the baseline with
  the curve brush

  \param painter Painter
  \param xMap x map
  \param yMap y map
  \param canvasRect Contents rect of the canvas
  \param polygon Polygon - will be modified !

  \sa setBrush(), setBaseline(), setStyle()
*/
void QwtPlotCurve::fillCurve( QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap,
    QPolygonF &polygon ) const
{
    if ( d_data->brush.style() == Qt::NoBrush )
        return;

    closePolyline( xMap, yMap, polygon );
    if ( polygon.count() <= 2 ) // a line can't be filled
        return;

    QBrush brush = d_data->brush;
    if ( !brush.color().isValid() )
        brush.setColor( d_data->pen.color() );

    painter->save();

    painter->setPen( Qt::NoPen );
    painter->setBrush( brush );

    painter->drawPolygon( polygon );

    painter->restore();
}

/*!
  \brief Complete a polygon to be a closed polygon including the 
         area between the original polygon and the baseline.

  \param painter Painter
  \param xMap X map
  \param yMap Y map
  \param polygon Polygon to be completed
*/
void QwtPlotCurve::closePolyline(
    const QwtScaleMap &xMap, const QwtScaleMap &yMap,
    QPolygonF &polygon ) const
{
    if ( polygon.size() < 2 )
        return;

    double baseline = d_data->baseline;
    
    if ( orientation() == Qt::Vertical )
    {
        if ( yMap.transformation()->type() == QwtScaleTransformation::Log10 )
        {
            if ( baseline < QwtScaleMap::LogMin )
                baseline = QwtScaleMap::LogMin;
        }

        double refY = yMap.transform( baseline );

        polygon += QPointF( polygon.last().x(), refY );
        polygon += QPointF( polygon.first().x(), refY );
    }
    else
    {
        if ( xMap.transformation()->type() == QwtScaleTransformation::Log10 )
        {
            if ( baseline < QwtScaleMap::LogMin )
                baseline = QwtScaleMap::LogMin;
        }

        double refX = xMap.transform( baseline );

        polygon += QPointF( refX, polygon.last().y() );
        polygon += QPointF( refX, polygon.first().y() );
    }
}

/*!
  Draw symbols

  \param painter Painter
  \param symbol Curve symbol
  \param xMap x map
  \param yMap y map
  \param canvasRect Contents rect of the canvas
  \param from Index of the first point to be painted
  \param to Index of the last point to be painted

  \sa setSymbol(), drawSeries(), drawCurve()
*/
void QwtPlotCurve::drawSymbols( QPainter *painter, const QwtSymbol &symbol,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap,
    const QRectF &canvasRect, int from, int to ) const
{
    const int chunkSize = 500;

    for ( int i = from; i <= to; i += chunkSize )
    {
        const int n = qMin( chunkSize, to - i + 1 );

        QPolygonF points;
        for ( int j = 0; j < n; j++ )
        {
            const QPointF sample = d_series->sample( i + j );

            const double xi = xMap.transform( sample.x() );
            const double yi = yMap.transform( sample.y() );

            if ( canvasRect.contains( xi, yi ) )
                points += QPointF( xi, yi );
        }

        if ( points.size() > 0 )
            symbol.drawSymbols( painter, points );
    }
}

/*!
  \brief Set the value of the baseline

  The baseline is needed for filling the curve with a brush or
  the Sticks drawing style.
  The interpretation of the baseline depends on the CurveType.
  With QwtPlotCurve::Yfx, the baseline is interpreted as a horizontal line
  at y = baseline(), with QwtPlotCurve::Yfy, it is interpreted as a vertical
  line at x = baseline().

  The default value is 0.0.

  \param value Value of the baseline
  \sa baseline(), setBrush(), setStyle(), setStyle()
*/
void QwtPlotCurve::setBaseline( double value )
{
    if ( d_data->baseline != value )
    {
        d_data->baseline = value;
        itemChanged();
    }
}

/*!
  \return Value of the baseline
  \sa setBaseline()
*/
double QwtPlotCurve::baseline() const
{
    return d_data->baseline;
}

/*!
  Find the closest curve point for a specific position

  \param pos Position, where to look for the closest curve point
  \param dist If dist != NULL, closestPoint() returns the distance between
              the position and the clostest curve point
  \return Index of the closest curve point, or -1 if none can be found
          ( f.e when the curve has no points )
  \note closestPoint() implements a dumb algorithm, that iterates
        over all points
*/
int QwtPlotCurve::closestPoint( const QPoint &pos, double *dist ) const
{
    if ( plot() == NULL || dataSize() <= 0 )
        return -1;

    const QwtScaleMap xMap = plot()->canvasMap( QwtPlot::xBottom );
    const QwtScaleMap yMap = plot()->canvasMap( QwtPlot::yLeft );

    int index = -1;
    double dmin = 1.0e10;

    for ( int i = 0; i < dataSize(); i++ )
    {
        const QPointF sample = d_series->sample( i );

        const double cx = xMap.transform( sample.x() ) - pos.x();
        const double cy = yMap.transform( sample.y() ) - pos.y();

        const double f = qwtSqr( cx ) + qwtSqr( cy );
        if ( f < dmin )
        {
            index = i;
            dmin = f;
        }
    }
    if ( dist )
        *dist = qSqrt( dmin );

    return index;
}

/*!
   \brief Update the widget that represents the item on the legend

   \param legend Legend
   \sa drawLegendIdentifier(), legendItem(), QwtPlotItem::Legend
*/
void QwtPlotCurve::updateLegend( QwtLegend *legend ) const
{
    if ( legend && !title().isNull()
        && ( d_data->legendAttributes & QwtPlotCurve::LegendShowSymbol )
        && d_data->symbol
        && d_data->symbol->style() != QwtSymbol::NoSymbol )
    {
        QWidget *lgdItem = legend->find( this );
        if ( lgdItem == NULL )
        {
            lgdItem = legendItem();
            if ( lgdItem )
                legend->insert( this, lgdItem );
        }
        if ( lgdItem && lgdItem->inherits( "QwtLegendItem" ) )
        {
            QwtLegendItem *l = ( QwtLegendItem * )lgdItem;
            l->setIdentifierSize( d_data->symbol->boundingSize() );
        }
    }

    QwtPlotItem::updateLegend( legend );
}

/*!
  \brief Draw the identifier representing the curve on the legend

  \param painter Painter
  \param rect Bounding rectangle for the identifier

  \sa setLegendAttribute(), QwtPlotItem::Legend
*/
void QwtPlotCurve::drawLegendIdentifier(
    QPainter *painter, const QRectF &rect ) const
{
    if ( rect.isEmpty() )
        return;

    const int dim = qMin( rect.width(), rect.height() );

    QSize size( dim, dim );

    QRectF r( 0, 0, size.width(), size.height() );
    r.moveCenter( rect.center() );

    if ( d_data->legendAttributes == 0 )
    {
        QBrush brush = d_data->brush;
        if ( brush.style() == Qt::NoBrush )
        {
            if ( style() != QwtPlotCurve::NoCurve )
                brush = QBrush( pen().color() );
            else if ( d_data->symbol &&
                ( d_data->symbol->style() != QwtSymbol::NoSymbol ) )
            {
                brush = QBrush( d_data->symbol->pen().color() );
            }
        }
        if ( brush.style() != Qt::NoBrush )
            painter->fillRect( r, brush );
    }
    if ( d_data->legendAttributes & QwtPlotCurve::LegendShowBrush )
    {
        if ( d_data->brush.style() != Qt::NoBrush )
            painter->fillRect( r, d_data->brush );
    }
    if ( d_data->legendAttributes & QwtPlotCurve::LegendShowLine )
    {
        if ( pen() != Qt::NoPen )
        {
            painter->setPen( pen() );
            painter->drawLine( rect.left(), rect.center().y(),
                               rect.right() - 1.0, rect.center().y() );
        }
    }
    if ( d_data->legendAttributes & QwtPlotCurve::LegendShowSymbol )
    {
        if ( d_data->symbol &&
            ( d_data->symbol->style() != QwtSymbol::NoSymbol ) )
        {
            QSize symbolSize = d_data->symbol->boundingSize();
            symbolSize -= QSize( 2, 2 );

            // scale the symbol size down if it doesn't fit into rect.

            double xRatio = 1.0;
            if ( rect.width() < symbolSize.width() )
                xRatio = rect.width() / symbolSize.width();
            double yRatio = 1.0;
            if ( rect.height() < symbolSize.height() )
                yRatio = rect.height() / symbolSize.height();

            const double ratio = qMin( xRatio, yRatio );

            painter->save();
            painter->scale( ratio, ratio );

            d_data->symbol->drawSymbol( painter, rect.center() / ratio );

            painter->restore();
        }
    }
}

/*!
  Initialize data with an array of points (explicitly shared).

  \param samples Vector of points
*/
void QwtPlotCurve::setSamples( const QVector<QPointF> &samples )
{
    delete d_series;
    d_series = new QwtPointSeriesData( samples );
    itemChanged();
}

/*!
  \brief Initialize the data by pointing to memory blocks which 
         are not managed by QwtPlotCurve.

  setRawSamples is provided for efficiency. 
  It is important to keep the pointers
  during the lifetime of the underlying QwtCPointerData class.

  \param xData pointer to x data
  \param yData pointer to y data
  \param size size of x and y

  \sa QwtCPointerData
*/
void QwtPlotCurve::setRawSamples( 
    const double *xData, const double *yData, int size )
{
    delete d_series;
    d_series = new QwtCPointerData( xData, yData, size );
    itemChanged();
}

/*!
  Set data by copying x- and y-values from specified memory blocks.
  Contrary to setRawSamples(), this function makes a 'deep copy' of
  the data.

  \param xData pointer to x values
  \param yData pointer to y values
  \param size size of xData and yData

  \sa QwtPointArrayData
*/
void QwtPlotCurve::setSamples( 
    const double *xData, const double *yData, int size )
{
    delete d_series;
    d_series = new QwtPointArrayData( xData, yData, size );
    itemChanged();
}

/*!
  \brief Initialize data with x- and y-arrays (explicitly shared)

  \param xData x data
  \param yData y data

  \sa QwtPointArrayData
*/
void QwtPlotCurve::setSamples( const QVector<double> &xData,
    const QVector<double> &yData )
{
    delete d_series;
    d_series = new QwtPointArrayData( xData, yData );
    itemChanged();
}
