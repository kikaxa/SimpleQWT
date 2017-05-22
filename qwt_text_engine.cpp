/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2003   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_text_engine.h"
#include "qwt_painter.h"
#include <qpainter.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qmap.h>
#include <qwidget.h>

class AscentCache
{
public:
    static int effectiveAscent( const QFont &font )
    {
        const QString fontKey = font.key();

        QMap<QString, int>::const_iterator it =
            d_ascentCache.find( fontKey );
        if ( it == d_ascentCache.end() )
        {
            int ascent = findAscent( font );
            it = d_ascentCache.insert( fontKey, ascent );
        }

        return ( *it );
    }

private:
    static int findAscent( const QFont &font )
    {
        static const QString dummy( "E" );
        static const QColor white( Qt::white );

        const QFontMetrics fm( font );
        QPixmap pm( fm.width( dummy ), fm.height() );
        pm.fill( white );

        QPainter p( &pm );
        p.setFont( font );
        p.drawText( 0, 0,  pm.width(), pm.height(), 0, dummy );
        p.end();

        const QImage img = pm.toImage();

        int row = 0;
        for ( row = 0; row < img.height(); row++ )
        {
            const QRgb *line = ( const QRgb * )img.scanLine( row );

            const int w = pm.width();
            for ( int col = 0; col < w; col++ )
            {
                if ( line[col] != white.rgb() )
                    return fm.ascent() - row + 1;
            }
        }

        return fm.ascent();
    }

    static QMap<QString, int> d_ascentCache;
};
QMap<QString, int> AscentCache::d_ascentCache;

/*!
   Find the height for a given width

   \param font Font of the text
   \param flags Bitwise OR of the flags used like in QPainter::drawText
   \param text Text to be rendered
   \param width Width

   \return Calculated height
*/
double QwtPlainTextEngine::heightForWidth( const QFont& font, int flags,
        const QString& text, double width )
{
    const QFontMetricsF fm( font );
    const QRectF rect = fm.boundingRect(
        QRectF( 0, 0, width, QWIDGETSIZE_MAX ), flags, text );

    return rect.height();
}

/*!
  Returns the size, that is needed to render text

  \param font Font of the text
  \param flags Bitwise OR of the flags used like in QPainter::drawText
  \param text Text to be rendered

  \return Caluclated size
*/
QSizeF QwtPlainTextEngine::textSize( const QFont &font,
    int flags, const QString& text )
{
    const QFontMetricsF fm( font );
    const QRectF rect = fm.boundingRect(
        QRectF( 0, 0, QWIDGETSIZE_MAX, QWIDGETSIZE_MAX ), flags, text );

    return rect.size();
}

/*!
  Return margins around the texts

  \param font Font of the text
  \param left Return 0
  \param right Return 0
  \param top Return value for the top margin
  \param bottom Return value for the bottom margin
*/
void QwtPlainTextEngine::textMargins( const QFont &font, const QString &,
    double &left, double &right, double &top, double &bottom )
{
    left = right = top = 0;

    const QFontMetricsF fm( font );
    top = fm.ascent() - AscentCache::effectiveAscent( font );
    bottom = fm.descent();
}

/*!
  \brief Draw the text in a clipping rectangle

  A wrapper for QPainter::drawText.

  \param painter Painter
  \param rect Clipping rectangle
  \param flags Bitwise OR of the flags used like in QPainter::drawText
  \param text Text to be rendered
*/
void QwtPlainTextEngine::draw( QPainter *painter, const QRectF &rect,
    int flags, const QString& text )
{
    painter->save();
    QwtPainter::unscaleFont( painter );
    painter->drawText( rect, flags, text );
    painter->restore();
}
