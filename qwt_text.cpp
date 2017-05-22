/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2003   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_text.h"
#include "qwt_text_engine.h"
#include <qmap.h>
#include <qfont.h>
#include <qcolor.h>
#include <qpen.h>
#include <qbrush.h>
#include <qpainter.h>
#include <qapplication.h>
#include <qdesktopwidget.h>
#include <qmath.h>

class QwtText::PrivateData
{
public:
    PrivateData():
        renderFlags( Qt::AlignCenter ),
        backgroundPen( Qt::NoPen ),
        backgroundBrush( Qt::NoBrush ),
        paintAttributes( 0 ),
        layoutAttributes( 0 )
    {
    }

    int renderFlags;
    QString text;
    QFont font;
    QColor color;
    QPen backgroundPen;
    QBrush backgroundBrush;

    QwtText::PaintAttributes paintAttributes;
    QwtText::LayoutAttributes layoutAttributes;
};

class QwtText::LayoutCache
{
public:
    void invalidate()
    {
        textSize = QSizeF();
    }

    QFont font;
    QSizeF textSize;
};

/*!
   Constructor

   \param text Text content
*/
QwtText::QwtText( const QString &text )
{
    d_data = new PrivateData;
    d_data->text = text;

    d_layoutCache = new LayoutCache;
}

//! Copy constructor
QwtText::QwtText( const QwtText &other )
{
    d_data = new PrivateData;
    *d_data = *other.d_data;

    d_layoutCache = new LayoutCache;
    *d_layoutCache = *other.d_layoutCache;
}

//! Destructor
QwtText::~QwtText()
{
    delete d_data;
    delete d_layoutCache;
}

//! Assignment operator
QwtText &QwtText::operator=( const QwtText & other )
{
    *d_data = *other.d_data;
    *d_layoutCache = *other.d_layoutCache;
    return *this;
}

//! Relational operator
bool QwtText::operator==( const QwtText &other ) const
{
    return d_data->renderFlags == other.d_data->renderFlags &&
        d_data->text == other.d_data->text &&
        d_data->font == other.d_data->font &&
        d_data->color == other.d_data->color &&
        d_data->backgroundPen == other.d_data->backgroundPen &&
        d_data->backgroundBrush == other.d_data->backgroundBrush &&
        d_data->paintAttributes == other.d_data->paintAttributes;
}

//! Relational operator
bool QwtText::operator!=( const QwtText &other ) const // invalidate
{
    return !( other == *this );
}

/*!
   Assign a new text content

   \param text Text content

   \sa text()
*/
void QwtText::setText( const QString &text )
{
    d_data->text = text;
    d_layoutCache->invalidate();
}

/*!
   Return the text.
   \sa setText()
*/
QString QwtText::text() const
{
    return d_data->text;
}

/*!
   \brief Change the render flags

   The default setting is Qt::AlignCenter

   \param renderFlags Bitwise OR of the flags used like in QPainter::drawText

   \sa renderFlags(), QwtTextEngine::draw()
*/
void QwtText::setRenderFlags( int renderFlags )
{
    if ( renderFlags != d_data->renderFlags )
    {
        d_data->renderFlags = renderFlags;
        d_layoutCache->invalidate();
    }
}

/*!
   \return Render flags
   \sa setRenderFlags()
*/
int QwtText::renderFlags() const
{
    return d_data->renderFlags;
}

/*!
   Set the font.

   \param font Font
   \note Setting the font might have no effect, when
         the text contains control sequences for setting fonts.
*/
void QwtText::setFont( const QFont &font )
{
    d_data->font = font;
    setPaintAttribute( PaintUsingTextFont );
}

//! Return the font.
QFont QwtText::font() const
{
    return d_data->font;
}

/*!
  Return the font of the text, if it has one.
  Otherwise return defaultFont.

  \param defaultFont Default font
  \sa setFont(), font(), PaintAttributes
*/
QFont QwtText::usedFont( const QFont &defaultFont ) const
{
    if ( d_data->paintAttributes & PaintUsingTextFont )
        return d_data->font;

    return defaultFont;
}

/*!
   Set the pen color used for painting the text.

   \param color Color
   \note Setting the color might have no effect, when
         the text contains control sequences for setting colors.
*/
void QwtText::setColor( const QColor &color )
{
    d_data->color = color;
    setPaintAttribute( PaintUsingTextColor );
}

//! Return the pen color, used for painting the text
QColor QwtText::color() const
{
    return d_data->color;
}

/*!
  Return the color of the text, if it has one.
  Otherwise return defaultColor.

  \param defaultColor Default color
  \sa setColor(), color(), PaintAttributes
*/
QColor QwtText::usedColor( const QColor &defaultColor ) const
{
    if ( d_data->paintAttributes & PaintUsingTextColor )
        return d_data->color;

    return defaultColor;
}

/*!
   Set the background pen

   \param pen Background pen
   \sa backgroundPen(), setBackgroundBrush()
*/
void QwtText::setBackgroundPen( const QPen &pen )
{
    d_data->backgroundPen = pen;
    setPaintAttribute( PaintBackground );
}

/*!
   \return Background pen
   \sa setBackgroundPen(), backgroundBrush()
*/
QPen QwtText::backgroundPen() const
{
    return d_data->backgroundPen;
}

/*!
   Set the background brush

   \param brush Background brush
   \sa backgroundBrush(), setBackgroundPen()
*/
void QwtText::setBackgroundBrush( const QBrush &brush )
{
    d_data->backgroundBrush = brush;
    setPaintAttribute( PaintBackground );
}

/*!
   \return Background brush
   \sa setBackgroundBrush(), backgroundPen()
*/
QBrush QwtText::backgroundBrush() const
{
    return d_data->backgroundBrush;
}

/*!
   Change a paint attribute

   \param attribute Paint attribute
   \param on On/Off

   \note Used by setFont(), setColor(),
         setBackgroundPen() and setBackgroundBrush()
   \sa testPaintAttribute()
*/
void QwtText::setPaintAttribute( PaintAttribute attribute, bool on )
{
    if ( on )
        d_data->paintAttributes |= attribute;
    else
        d_data->paintAttributes &= ~attribute;
}

/*!
   Test a paint attribute

   \param attribute Paint attribute
   \return true, if attribute is enabled

   \sa setPaintAttribute()
*/
bool QwtText::testPaintAttribute( PaintAttribute attribute ) const
{
    return d_data->paintAttributes & attribute;
}

/*!
   Change a layout attribute

   \param attribute Layout attribute
   \param on On/Off
   \sa testLayoutAttribute()
*/
void QwtText::setLayoutAttribute( LayoutAttribute attribute, bool on )
{
    if ( on )
        d_data->layoutAttributes |= attribute;
    else
        d_data->layoutAttributes &= ~attribute;
}

/*!
   Test a layout attribute

   \param attribute Layout attribute
   \return true, if attribute is enabled

   \sa setLayoutAttribute()
*/
bool QwtText::testLayoutAttribute( LayoutAttribute attribute ) const
{
    return d_data->layoutAttributes | attribute;
}

/*!
   Find the height for a given width

   \param defaultFont Font, used for the calculation if the text has no font
   \param width Width

   \return Calculated height
*/
double QwtText::heightForWidth( double width, const QFont &defaultFont ) const
{
    // We want to calculate in screen metrics. So
    // we need a font that uses screen metrics

    const QFont font( usedFont( defaultFont ), QApplication::desktop() );

    double h = 0;

    if ( d_data->layoutAttributes & MinimumLayout )
    {
        double left, right, top, bottom;
        QwtPlainTextEngine::textMargins( font, d_data->text,
            left, right, top, bottom );

        h = QwtPlainTextEngine::heightForWidth(
            font, d_data->renderFlags, d_data->text,
            width + left + right );

        h -= top + bottom;
    }
    else
    {
        h = QwtPlainTextEngine::heightForWidth(
            font, d_data->renderFlags, d_data->text, width );
    }

    return h;
}

/*!
   Find the height for a given width

   \param defaultFont Font, used for the calculation if the text has no font

   \return Calculated height
*/

/*!
   Returns the size, that is needed to render text

   \param defaultFont Font of the text
   \return Caluclated size
*/
QSizeF QwtText::textSize( const QFont &defaultFont ) const
{
    // We want to calculate in screen metrics. So
    // we need a font that uses screen metrics

    const QFont font( usedFont( defaultFont ), QApplication::desktop() );

    if ( !d_layoutCache->textSize.isValid()
        || d_layoutCache->font != font )
    {
        d_layoutCache->textSize = QwtPlainTextEngine::textSize(
            font, d_data->renderFlags, d_data->text );
        d_layoutCache->font = font;
    }

    QSizeF sz = d_layoutCache->textSize;

    if ( d_data->layoutAttributes & MinimumLayout )
    {
        double left, right, top, bottom;
        QwtPlainTextEngine::textMargins( font, d_data->text,
            left, right, top, bottom );
        sz -= QSizeF( left + right, top + bottom );
    }

    return sz;
}

/*!
   Draw a text into a rectangle

   \param painter Painter
   \param rect Rectangle
*/
void QwtText::draw( QPainter *painter, const QRectF &rect ) const
{
    if ( d_data->paintAttributes & PaintBackground )
    {
        if ( d_data->backgroundPen != Qt::NoPen ||
            d_data->backgroundBrush != Qt::NoBrush )
        {
            painter->save();
            painter->setPen( d_data->backgroundPen );
            painter->setBrush( d_data->backgroundBrush );
            painter->drawRect( rect );
            painter->restore();
        }
    }

    painter->save();

    if ( d_data->paintAttributes & PaintUsingTextFont )
    {
        painter->setFont( d_data->font );
    }

    if ( d_data->paintAttributes & PaintUsingTextColor )
    {
        if ( d_data->color.isValid() )
            painter->setPen( d_data->color );
    }

    QRectF expandedRect = rect;
    if ( d_data->layoutAttributes & MinimumLayout )
    {
        // We want to calculate in screen metrics. So
        // we need a font that uses screen metrics

        const QFont font( painter->font(), QApplication::desktop() );

        double left, right, top, bottom;
        QwtPlainTextEngine::textMargins(
            font, d_data->text, left, right, top, bottom );

        expandedRect.setTop( rect.top() - top );
        expandedRect.setBottom( rect.bottom() + bottom );
        expandedRect.setLeft( rect.left() - left );
        expandedRect.setRight( rect.right() + right );
    }

    QwtPlainTextEngine::draw( painter, expandedRect,
        d_data->renderFlags, d_data->text );

    painter->restore();
}
