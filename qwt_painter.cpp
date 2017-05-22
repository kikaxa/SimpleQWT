/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_painter.h"
#include "qwt_color_map.h"
#include "qwt_scale_map.h"
#include <qwidget.h>
#include <qrect.h>
#include <qpainter.h>
#include <qpaintdevice.h>
#include <qpixmap.h>
#include <qstyle.h>
#include <qstyleoption.h>
#include <qapplication.h>
#include <qdesktopwidget.h>


void QwtPainter::unscaleFont( QPainter *painter )
{
    if ( painter->font().pixelSize() >= 0 )
        return;

    static QSize screenResolution;
    if ( !screenResolution.isValid() )
    {
        QDesktopWidget *desktop = QApplication::desktop();
        if ( desktop )
        {
            screenResolution.setWidth( desktop->logicalDpiX() );
            screenResolution.setHeight( desktop->logicalDpiY() );
        }
    }

    const QPaintDevice *pd = painter->device();
    if ( pd->logicalDpiX() != screenResolution.width() ||
        pd->logicalDpiY() != screenResolution.height() )
    {
        QFont pixelFont( painter->font(), QApplication::desktop() );
        pixelFont.setPixelSize( QFontInfo( pixelFont ).pixelSize() );

        painter->setFont( pixelFont );
    }
}

//! Wrapper for QPainter::drawImage()
void QwtPainter::drawImage( QPainter *painter,
    const QRectF &rect, const QImage &image )
{
    const QRect alignedRect = rect.toAlignedRect();

    if ( alignedRect != rect )
    {
        const QRectF clipRect = rect.adjusted( 0.0, 0.0, -1.0, -1.0 );

        painter->save();
        painter->setClipRect( clipRect, Qt::IntersectClip );
        painter->drawImage( alignedRect, image );
        painter->restore();
    }
    else
    {
        painter->drawImage( alignedRect, image );
    }
}

//! Draw a focus rectangle on a widget using its style.
void QwtPainter::drawFocusRect( QPainter *painter, QWidget *widget,
    const QRect &rect )
{
    QStyleOptionFocusRect opt;
    opt.init( widget );
    opt.rect = rect;
    opt.state |= QStyle::State_HasFocus;

    widget->style()->drawPrimitive( QStyle::PE_FrameFocusRect,
        &opt, painter, widget );
}

/*!
  Draw a color bar into a rectangle

  \param painter Painter
  \param colorMap Color map
  \param interval Value range
  \param scaleMap Scale map
  \param orientation Orientation
  \param rect Traget rectangle
*/
void QwtPainter::drawColorBar( QPainter *painter,
        const QwtColorMap &colorMap, const QwtInterval &interval,
        const QwtScaleMap &scaleMap, Qt::Orientation orientation,
        const QRectF &rect )
{
    QColor c;

    const QRect devRect = rect.toAlignedRect();

    /*
      We paint to a pixmap first to have something scalable for printing
      ( f.e. in a Pdf document )
     */

    QPixmap pixmap( devRect.size() );
    QPainter pmPainter( &pixmap );
    pmPainter.translate( -devRect.x(), -devRect.y() );

    if ( orientation == Qt::Horizontal )
    {
        QwtScaleMap sMap = scaleMap;
        sMap.setPaintInterval( rect.left(), rect.right() );

        for ( int x = devRect.left(); x <= devRect.right(); x++ )
        {
            const double value = sMap.invTransform( x );

            c.setRgb( colorMap.rgb( interval, value ) );

            pmPainter.setPen( c );
            pmPainter.drawLine( x, devRect.top(), x, devRect.bottom() );
        }
    }
    else // Vertical
    {
        QwtScaleMap sMap = scaleMap;
        sMap.setPaintInterval( rect.bottom(), rect.top() );

        for ( int y = devRect.top(); y <= devRect.bottom(); y++ )
        {
            const double value = sMap.invTransform( y );

            c.setRgb( colorMap.rgb( interval, value ) );

            pmPainter.setPen( c );
            pmPainter.drawLine( devRect.left(), y, devRect.right(), y );
        }
    }
    pmPainter.end();

    const QRect alignedRect = rect.toAlignedRect();

    if ( alignedRect != rect )
    {
        const QRectF clipRect = rect.adjusted( 0.0, 0.0, -1.0, -1.0 );

        painter->save();
        painter->setClipRect( clipRect, Qt::IntersectClip );
        painter->drawPixmap( alignedRect, pixmap );
        painter->restore();
    }
    else
    {
        painter->drawPixmap( alignedRect, pixmap );
    }
}
