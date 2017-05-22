/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_scale_draw.h"
#include "qwt_scale_div.h"
#include "qwt_scale_map.h"
#include <qpen.h>
#include <qpainter.h>
#include <qmath.h>

/*!
  \brief Constructor

  The range of the scale is initialized to [0, 100],
  The position is at (0, 0) with a length of 100.
  The orientation is QwtAbstractScaleDraw::Bottom.
*/
QwtScaleDraw::QwtScaleDraw():
    len( 0 ),
    alignment( QwtScaleDraw::BottomScale )
{
    Multiplier = 1.;
    setLength( 100 );
}

/*!
   Set the alignment of the scale

   The default alignment is QwtScaleDraw::BottomScale
   \sa alignment()
*/
void QwtScaleDraw::setAlignment( Alignment align )
{
    alignment = align;
}

/*!
  Return the orientation

  TopScale, BottomScale are horizontal (Qt::Horizontal) scales,
  LeftScale, RightScale are vertical (Qt::Vertical) scales.

  \sa alignment()
*/
Qt::Orientation QwtScaleDraw::orientation() const
{
    switch ( alignment )
    {
        case TopScale:
        case BottomScale:
            return Qt::Horizontal;
        case LeftScale:
        case RightScale:
        default:
            return Qt::Vertical;
    }
}

/*!
   Calculate the width/height that is needed for a
   vertical/horizontal scale.

   The extent is calculated from the pen width of the backbone,
   the major tick length, the spacing and the maximum width/height
   of the labels.

   \param font Font used for painting the labels

   \sa minLength()
*/
double QwtScaleDraw::extent( const QFont &font ) const
{
    double d = 0;

    if ( orientation() == Qt::Vertical )
        d = maxLabelWidth( font );
    else
        d = maxLabelHeight( font );

    d += spacing();
    d += maxTickLength();

    const double pw = qMax( 1, penWidth() );  // penwidth can be zero
    d += pw;

    return d;
}

/*!
   Find the position, where to paint a label

   The position has a distance of majTickLength() + spacing() + 1
   from the backbone. The direction depends on the alignment()

   \param value Value
*/
QPointF QwtScaleDraw::labelPosition( double value ) const
{
    const double tval = scaleMap().transform( value );
    double dist = spacing() + qMax( 1, penWidth() );
    dist += tickLength( QwtScaleDiv::MajorTick );

    double px = 0;
    double py = 0;

    switch ( alignment )
    {
        case RightScale:
        {
            px = pos.x() + dist;
            py = tval;
            break;
        }
        case LeftScale:
        {
            px = pos.x() - dist;
            py = tval;
            break;
        }
        case BottomScale:
        {
            px = tval;
            py = pos.y() + dist;
            break;
        }
        case TopScale:
        {
            px = tval;
            py = pos.y() - dist;
            break;
        }
    }

    return QPointF( px, py );
}

/*!
   Draw a tick

   \param painter Painter
   \param value Value of the tick
   \param len Lenght of the tick

   \sa drawBackbone(), drawLabel()
*/
void QwtScaleDraw::drawTick( QPainter *painter, double value, double len ) const
{
    double tval = scaleMap().transform( value );
    const int pw = penWidth();

    switch ( alignment )
    {
        case LeftScale:
        {
            double x1 = pos.x();
            double x2 = x1 - pw - len;

            painter->drawLine( x1, tval, x2, tval );
            break;
        }

        case RightScale:
        {
            double x1 = pos.x();
            double x2 = x1 + pw + len;

            painter->drawLine( x1, tval, x2, tval );
            break;
        }

        case BottomScale:
        {
            double y1 = pos.y();
            double y2 = y1 + pw + len;

            painter->drawLine( tval, y1, tval, y2 );
            break;
        }

        case TopScale:
        {
            double y1 = pos.y();
            double y2 = y1 - pw - len;

            painter->drawLine( tval, y1, tval, y2 );
            break;
        }
    }
}

/*!
   Draws the baseline of the scale
   \param painter Painter

   \sa drawTick(), drawLabel()
*/
void QwtScaleDraw::drawBackbone( QPainter *painter ) const
{
    // pos indicates a border not the center of the backbone line
    // so we need to shift its position depending on the pen width
    // and the alignment of the scale

    double off = 0.5 * penWidth();

    switch ( alignment )
    {
        case LeftScale:
        {
            double x = pos.x() - off;

            painter->drawLine( x, pos.y(), x, pos.y() + len );
            break;
        }
        case RightScale:
        {
            double x = pos.x() + off;

            painter->drawLine( x, pos.y(), x, pos.y() + len );
            break;
        }
        case TopScale:
        {
            double y = pos.y() - off;

            painter->drawLine( pos.x(), y, pos.x() + len, y );
            break;
        }
        case BottomScale:
        {
            double y = pos.y() + off;

            painter->drawLine( pos.x(), y, pos.x() + len, y );
            break;
        }
    }
}

/*!
  \brief Move the position of the scale

  The meaning of the parameter pos depends on the alignment:
  <dl>
  <dt>QwtScaleDraw::LeftScale
  <dd>The origin is the topmost point of the
      backbone. The backbone is a vertical line.
      Scale marks and labels are drawn
      at the left of the backbone.
  <dt>QwtScaleDraw::RightScale
  <dd>The origin is the topmost point of the
      backbone. The backbone is a vertical line.
      Scale marks and labels are drawn
      at the right of the backbone.
  <dt>QwtScaleDraw::TopScale
  <dd>The origin is the leftmost point of the
      backbone. The backbone is a horizontal line.
      Scale marks and labels are drawn
      above the backbone.
  <dt>QwtScaleDraw::BottomScale
  <dd>The origin is the leftmost point of the
      backbone. The backbone is a horizontal line
      Scale marks and labels are drawn
      below the backbone.
  </dl>

  \param pos Origin of the scale

  \sa pos(), setLength()
*/
void QwtScaleDraw::move( const QPointF &_pos )
{
    pos = _pos;
    updateMap();
}

/*!
  Set the length of the backbone.

  The length doesn't include the space needed for
  overlapping labels.

  \sa move()
*/
void QwtScaleDraw::setLength( double length )
{
    len = length;
    updateMap();
}

/*!
   Draws the label for a major scale tick

   \param painter Painter
   \param value Value

   \sa drawTick(), drawBackbone()
*/
void QwtScaleDraw::drawLabel( QPainter *painter, double value ) const
{
    QwtText lbl = tickLabel( value / Multiplier );
    if ( lbl.isEmpty() )
        return;

    QPointF poss = labelPosition( value );
    QSizeF labelSize = lbl.textSize( painter->font() );
    poss += labelOffset( labelSize );

    //confine label inside rect()
    if (orientation() == Qt::Horizontal) {
        if (poss.x() <= pos.x())
            poss.rx() = pos.x() + 1;
        if (poss.x() + labelSize.width() >= pos.x() + len)
            poss.rx() = pos.x() + len - 1 - labelSize.width();
    } else {
        if (poss.y() <= pos.y())
            poss.ry() = pos.y() + 1;
        if (poss.y() + labelSize.height() >= pos.y() + len)
            poss.ry() = pos.y() + len - 1 - labelSize.height();
    }

    lbl.draw ( painter, QRect( poss.toPoint(), labelSize.toSize() ) );
}

/*!
   Calculate the transformation that is needed to paint a label
   depending on its alignment.

   \param size Size of the label
*/
QPointF QwtScaleDraw::labelOffset( const QSizeF &size ) const
{
    double x, y;
    switch ( alignment )
    {
    case RightScale:
        x = 0;
        y = -( 0.5 * size.height() );
        break;
    case LeftScale:
        x = -size.width();
        y = -( 0.5 * size.height() );
        break;
    case BottomScale:
        x = -( 0.5 * size.width() );
        y = 0;
        break;
    case TopScale:
        x = -( 0.5 * size.width() );
        y = -size.height();
        break;
    }

    return QPointF(x, y);
}

/*!
   Calculate the size that is needed to draw a label

   \param font Label font
   \param value Value
*/
QSizeF QwtScaleDraw::labelSize( const QFont &font, double value ) const
{
    QwtText lbl = tickLabel( value / Multiplier );
    if ( lbl.isEmpty() )
        return QSizeF( 0, 0 );

    const QSizeF labelSize = lbl.textSize( font );
    return labelSize;
}

/*!
  \param font Font
  \return the maximum width of a label
*/
int QwtScaleDraw::maxLabelWidth( const QFont &font ) const
{
    int maxWidth = 0;

    const QList<double> &ticks = scaleDiv().ticks( QwtScaleDiv::MajorTick );
    for ( uint i = 0; i < ( uint )ticks.count(); i++ )
    {
        const int w = labelSize( font, ticks[i] ).width();
        if ( w > maxWidth )
            maxWidth = w;

        if (i == 1) //optimize: only first and last 2 ticks
            i = qMax(1, ticks.count()-1 -2);
    }

    return maxWidth;
}

/*!
  \param font Font
  \return the maximum height of a label
*/
int QwtScaleDraw::maxLabelHeight( const QFont &font ) const
{
    int maxHeight = 0;

    const QList<double> &ticks = scaleDiv().ticks( QwtScaleDiv::MajorTick );
    for ( uint i = 0; i < ( uint )ticks.count(); i++ )
    {
        const int h = labelSize( font, ticks[i] ).height();
        if ( h > maxHeight )
            maxHeight = h;

        break; //optimize: all one-liners
    }

    return maxHeight;
}

void QwtScaleDraw::updateMap()
{
    QwtScaleMap &sm = scaleMap();
    if ( orientation() == Qt::Vertical )
        sm.setPaintInterval( pos.y() + len, pos.y() );
    else
        sm.setPaintInterval( pos.x(), pos.x() + len );
}
