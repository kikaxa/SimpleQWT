/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_plot_rasteritem.h"
#include "qwt_legend.h"
#include "qwt_legend_item.h"
#include "qwt_scale_map.h"
#include "qwt_painter.h"
#include <qapplication.h>
#include <qdesktopwidget.h>
#include <qpainter.h>
#include <qpaintengine.h>
#include <float.h>

class QwtPlotRasterItem::PrivateData
{
public:
    PrivateData():
        alpha( -1 ),
        paintAttributes( QwtPlotRasterItem::PaintInDeviceResolution )
    {
    }

    int alpha;
    QwtPlotRasterItem::PaintAttributes paintAttributes;
};


static QRectF qwtAlignRect(const QRectF &rect)
{
    QRectF r;
    r.setLeft( qRound( rect.left() ) );
    r.setRight( qRound( rect.right() ) );
    r.setTop( qRound( rect.top() ) );
    r.setBottom( qRound( rect.bottom() ) );

    return r;
}

static void qwtExpandImage(QImage &image,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap,
    const QRectF &area, const QRectF &area2, const QRectF &paintRect )
{
    if (image.format() != QImage::Format_ARGB32)
        image = image.convertToFormat(QImage::Format_ARGB32);

    const QSize sz = paintRect.toRect().size();

    const int w = image.width();
    const int h = image.height();

    const QRectF r = QwtScaleMap::transform(xMap, yMap, area).normalized();
    const double pw = ( r.width() - 1) / w;
    const double ph = ( r.height() - 1) / h;

    double px0, py0;
    if ( !xMap.isInverting() )
    {
        px0 = xMap.transform( area2.left() );
        px0 = qRound( px0 );
        px0 = px0 - xMap.transform( area.left() );
    }
    else
    {
        px0 = xMap.transform( area2.right() );
        px0 = qRound( px0 );
        px0 -= xMap.transform( area.right() );

        px0 -= 1.0;
    }

    if ( !yMap.isInverting() )
    {
        py0 = yMap.transform( area2.top() );
        py0 = qRound( py0 );
        py0 -= yMap.transform( area.top() );
    }
    else
    {
        py0 = yMap.transform( area2.bottom() );
        py0 = qRound( py0 );
        py0 -= yMap.transform( area.bottom() );

        py0 -= 1.0;
    }


    QImage expanded(sz, QImage::Format_ARGB32);

    for ( int y1 = 0; y1 < h; y1++ )
    {
        int yy1;
        if ( y1 == 0 )
        {
            yy1 = 0;
        }
        else
        {
            yy1 = qRound( y1 * ph - py0 );
            if ( yy1 < 0 )
                yy1 = 0;
        }

        int yy2;
        if ( y1 == h - 1 )
        {
            yy2 = sz.height();
        }
        else
        {
            yy2 = qRound( ( y1 + 1 ) * ph - py0 );
            if ( yy2 > sz.height() )
                yy2 = sz.height();
        }

        const quint32 *line1 = (const quint32 *) image.scanLine( y1 );

        for ( int x1 = 0; x1 < w; x1++ )
        {
            int xx1;
            if ( x1 == 0 )
            {
                xx1 = 0;
            }
            else
            {
                xx1 = qRound( x1 * pw - px0 );
                if ( xx1 < 0 )
                    xx1 = 0;
            }

            int xx2;
            if ( x1 == w - 1 )
            {
                xx2 = sz.width();
            }
            else
            {
                xx2 = qRound( ( x1 + 1 ) * pw - px0 );
                if ( xx2 > sz.width() )
                    xx2 = sz.width();
            }

            const quint32 rgb( line1[x1] );
            for ( int y2 = yy1; y2 < yy2; y2++ )
            {
                quint32 *line2 = ( quint32 *) expanded.scanLine( y2 );
                for ( int x2 = xx1; x2 < xx2; x2++ )
                    line2[x2] = rgb;
            }
        }
    }
    
    image = expanded;
}   

static QRectF expandToPixels(const QRectF &rect, const QRectF &pixelRect)
{
    const double pw = pixelRect.width();
    const double ph = pixelRect.height();

    const double dx1 = pixelRect.left() - rect.left();
    const double dx2 = pixelRect.right() - rect.right();
    const double dy1 = pixelRect.top() - rect.top();
    const double dy2 = pixelRect.bottom() - rect.bottom();

    QRectF r;
    r.setLeft( pixelRect.left() - qCeil( dx1 / pw ) * pw );
    r.setTop( pixelRect.top() - qCeil( dy1 / ph ) * ph );
    r.setRight( pixelRect.right() - qFloor( dx2 / pw ) * pw );
    r.setBottom( pixelRect.bottom() - qFloor( dy2 / ph ) * ph );

    return r;
}

static void transformMaps( const QTransform &tr,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap,
    QwtScaleMap &xxMap, QwtScaleMap &yyMap )
{
    const QPointF p1 = tr.map( QPointF( xMap.p1(), yMap.p1() ) );
    const QPointF p2 = tr.map( QPointF( xMap.p2(), yMap.p2() ) );

    xxMap = xMap;
    xxMap.setPaintInterval( p1.x(), p2.x() );

    yyMap = yMap;
    yyMap.setPaintInterval( p1.y(), p2.y() );
}

static void toRgba( QImage& image, int alpha )
{
    if (image.format() != QImage::Format_ARGB32)
        image = image.convertToFormat(QImage::Format_ARGB32);

    const QRgb mask1 = qRgba( 0, 0, 0, alpha );
    const QRgb mask2 = qRgba( 255, 255, 255, 0 );
    const QRgb mask3 = qRgba( 0, 0, 0, 255 );

    for ( int y = 0; y < image.height(); y++ )
    {
        QRgb* line = ( QRgb* ) image.scanLine( y );

        for ( int x = 0; x < image.width(); x++ )
        {
            const QRgb rgb = *line;
            if ( rgb & mask3 ) // alpha != 0
                *line = ( rgb & mask2 ) | mask1;
            line++;
        }
    }
}

//! Constructor
QwtPlotRasterItem::QwtPlotRasterItem( const QString& title ):
    QwtPlotItem( QwtText( title ) )
{
    init();
}

//! Constructor
QwtPlotRasterItem::QwtPlotRasterItem( const QwtText& title ):
    QwtPlotItem( title )
{
    init();
}

//! Destructor
QwtPlotRasterItem::~QwtPlotRasterItem()
{
    delete d_data;
}

void QwtPlotRasterItem::init()
{
    d_data = new PrivateData();

    setZ( 8.0 );
}

/*!
  Specify an attribute how to draw the raster item

  \param attribute Paint attribute
  \param on On/Off
  /sa PaintAttribute, testPaintAttribute()
*/
void QwtPlotRasterItem::setPaintAttribute( PaintAttribute attribute, bool on )
{
    if ( on )
        d_data->paintAttributes |= attribute;
    else
        d_data->paintAttributes &= ~attribute;
}

/*!
    \brief Return the current paint attributes
    \sa PaintAttribute, setPaintAttribute()
*/
bool QwtPlotRasterItem::testPaintAttribute( PaintAttribute attribute ) const
{
    return ( d_data->paintAttributes & attribute );
}

/*!
   \brief Set an alpha value for the raster data

   Often a plot has several types of raster data organized in layers.
   ( f.e a geographical map, with weather statistics ).
   Using setAlpha() raster items can be stacked easily.

   The alpha value is a value [0, 255] to
   control the transparency of the image. 0 represents a fully
   transparent color, while 255 represents a fully opaque color.

   \param alpha Alpha value

   - alpha >= 0\n
     All alpha values of the pixels returned by renderImage() will be set to
     alpha, beside those with an alpha value of 0 (invalid pixels).
   - alpha < 0
     The alpha values returned by renderImage() are not changed.

   The default alpha value is -1.

   \sa alpha()
*/
void QwtPlotRasterItem::setAlpha( int alpha )
{
    if ( alpha < 0 )
        alpha = -1;

    if ( alpha > 255 )
        alpha = 255;

    if ( alpha != d_data->alpha )
    {
        d_data->alpha = alpha;

        itemChanged();
    }
}

/*!
  \return Alpha value of the raster item
  \sa setAlpha()
*/
int QwtPlotRasterItem::alpha() const
{
    return d_data->alpha;
}

/*!
   \brief Pixel hint

   The geometry of a pixel is used to calculated the resolution and
   alignment of the rendered image. 

   Width and height of the hint need to be the horizontal  
   and vertical distances between 2 neighboured points. 
   The center of the hint has to be the position of any point 
   ( it doesn't matter which one ).

   Limiting the resolution of the image might significantly improve
   the performance and heavily reduce the amount of memory when rendering
   a QImage from the raster data. 

   The default implementation returns an empty rectangle (QRectF()),
   meaning, that the image will be rendered in target device ( f.e screen )
   resolution.

   \param area In most implementations the resolution of the data doesn't
               depend on the requested area.

   \return Bounding rectangle of a pixel

   \sa render(), renderImage()
*/
QRectF QwtPlotRasterItem::pixelHint( const QRectF &area ) const
{
    Q_UNUSED( area );
    return QRectF();
}

/*!
  \brief Draw the raster data
  \param painter Painter
  \param xMap X-Scale Map
  \param yMap Y-Scale Map
  \param canvasRect Contents rect of the plot canvas
*/
void QwtPlotRasterItem::draw( QPainter *painter,
    const QwtScaleMap &xMap, const QwtScaleMap &yMap,
    const QRectF &canvasRect ) const
{
    if ( canvasRect.isEmpty() || d_data->alpha == 0 )
        return;

    const QwtInterval xInterval = interval( Qt::XAxis );
    const QwtInterval yInterval = interval( Qt::YAxis );

    /*
        Scaling a rastered image always results in a loss of
        precision/quality. So we always render the image in
        paint device resolution.
    */

    QwtScaleMap xxMap, yyMap;
    transformMaps( painter->transform(), xMap, yMap, xxMap, yyMap );

    QRectF paintRect = painter->transform().mapRect( canvasRect );
    QRectF area = QwtScaleMap::invTransform( xxMap, yyMap, paintRect );

    const QRectF br = boundingRect();
    if ( br.isValid() && !br.contains( area ) )
    {
        area &= br;
        if ( !area.isValid() )
            return;

        paintRect = QwtScaleMap::transform( xxMap, yyMap, area );
    }
    paintRect = qwtAlignRect(paintRect);

    QRectF pixelRect = pixelHint(area);
    if ( !pixelRect.isEmpty() )
    {
        // pixel in target device resolution 
        const double dx = fabs( xxMap.invTransform( 1 ) - xxMap.invTransform( 0 ) );
        const double dy = fabs( yyMap.invTransform( 1 ) - yyMap.invTransform( 0 ) );

        if ( dx > pixelRect.width() && dy > pixelRect.height() )
        {
            /*
              When the resolution of the data pixels is higher than
              the resolution of the target device we render in
              target device resolution.
             */
            pixelRect = QRectF();
        }
    }

    QImage image;
    if ( pixelRect.isEmpty() )
    {
        // When we have no information about position and size of
        // data pixels we render in resolution of the paint device.

        image = compose(xxMap, yyMap, 
            area, paintRect, paintRect.size().toSize());
        if ( image.isNull() )
            return;
    }
    else
    {
        // align the area to the data pixels
        QRectF imageArea = expandToPixels(area, pixelRect);

        if ( imageArea.right() == xInterval.maxValue() )
            imageArea.adjust(0, 0, pixelRect.width(), 0);

        if ( imageArea.bottom() == yInterval.maxValue() )
            imageArea.adjust(0, 0, 0, pixelRect.height() );

        QSize imageSize;
        imageSize.setWidth( qRound( imageArea.width() / pixelRect.width() ) );
        imageSize.setHeight( qRound( imageArea.height() / pixelRect.height() ) );
        image = compose(xxMap, yyMap, 
            imageArea, paintRect, imageSize );
        if ( image.isNull() )
            return;

        if ( ( image.width() > 1 || image.height() > 1 ) &&
            testPaintAttribute( PaintInDeviceResolution ) )
        {
            // Because of rounding errors the pixels 
            // need to be expanded manually to rectangles of 
            // different sizes

            qwtExpandImage(image, xxMap, yyMap,
                           imageArea, area, paintRect );
        }
    }

    painter->save();
    painter->setWorldTransform( QTransform() );
    
    QwtPainter::drawImage( painter, paintRect, image );

    painter->restore();
}

/*!
   \return Bounding interval for an axis

   This method is intended to be reimplemented by derived classes.
   The default implementation returns an invalid interval.
   
   \param axis X, Y, or Z axis
*/
QwtInterval QwtPlotRasterItem::interval(Qt::Axis axis) const
{
    Q_UNUSED( axis );
    return QwtInterval();
}

/*!
   \return Bounding rect of the data
   \sa QwtPlotRasterItem::interval()
*/
QRectF QwtPlotRasterItem::boundingRect() const
{
    const QwtInterval intervalX = interval( Qt::XAxis );
    const QwtInterval intervalY = interval( Qt::YAxis );

    if ( !intervalX.isValid() && !intervalY.isValid() )
        return QRectF(); // no bounding rect

    QRectF r;

    if ( intervalX.isValid() )
    {
        r.setLeft( intervalX.minValue() );
        r.setRight( intervalX.maxValue() );
    }
    else
    {
        r.setLeft(-0.5 * FLT_MAX);
        r.setWidth(FLT_MAX);
    }

    if ( intervalY.isValid() )
    {
        r.setTop( intervalY.minValue() );
        r.setBottom( intervalY.maxValue() );
    }
    else
    {
        r.setTop(-0.5 * FLT_MAX);
        r.setHeight(FLT_MAX);
    }

    return r.normalized();
}

QImage QwtPlotRasterItem::compose( 
    const QwtScaleMap &xMap, const QwtScaleMap &yMap,
    const QRectF &imageArea, const QRectF &paintRect, 
    const QSize &imageSize) const
{
    QImage image;
    if ( imageArea.isEmpty() || paintRect.isEmpty() || imageSize.isEmpty() )
        return image;

    double dx = 0.0;
    if ( paintRect.toRect().width() > imageSize.width() )
        dx = imageArea.width() / imageSize.width();

    const QwtScaleMap xxMap =
        imageMap(Qt::Horizontal, xMap, imageArea, imageSize, dx);

    double dy = 0.0;
    if ( paintRect.toRect().height() > imageSize.height() )
        dy = imageArea.height() / imageSize.height();

    const QwtScaleMap yyMap =
        imageMap(Qt::Vertical, yMap, imageArea, imageSize, dy);

    image = renderImage( xxMap, yyMap, imageArea, imageSize );

    if ( d_data->alpha >= 0 && d_data->alpha < 255 )
        toRgba( image, d_data->alpha );

    return image;
}

/*!
   \brief Calculate a scale map for painting to an image

   \param orientation Orientation, Qt::Horizontal means a X axis
   \param map Scale map for rendering the plot item
   \param area Area to be painted on the image
   \param imageSize Image size
   \param pixelSize Width/Height of a data pixel
*/
QwtScaleMap QwtPlotRasterItem::imageMap(
    Qt::Orientation orientation,
    const QwtScaleMap &map, const QRectF &area,
    const QSize &imageSize, double pixelSize) const
{
    double p1, p2, s1, s2;

    if ( orientation == Qt::Horizontal )
    {
        p1 = 0.0;
        p2 = imageSize.width();
        s1 = area.left();
        s2 = area.right();
    }
    else
    {
        p1 = 0.0;
        p2 = imageSize.height();
        s1 = area.top();
        s2 = area.bottom();
    }

    if ( pixelSize > 0.0 )
    {
        double off = 0.5 * pixelSize;
        if ( map.isInverting() )
            off = -off;

        s1 += off;
        s2 += off;
    }
    else
    {
        p2--;
    }

    if ( map.isInverting() && ( s1 < s2 ) )
        qSwap( s1, s2 );

    QwtScaleMap newMap = map;
    newMap.setPaintInterval( p1, p2 );
    newMap.setScaleInterval( s1, s2 );

    return newMap;
}
