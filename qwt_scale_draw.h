#pragma once

#include "qwt_abstract_scale_draw.h"
#include <qpoint.h>
#include <qrect.h>
#include <qtransform.h>

/*!
  \brief A class for drawing scales

  QwtScaleDraw can be used to draw linear or logarithmic scales.
  A scale has a position, an alignment and a length, which can be specified .

  After a scale division has been specified as a QwtScaleDiv object
  using QwtAbstractScaleDraw::setScaleDiv(const QwtScaleDiv &s),
  the scale can be drawn with the QwtAbstractScaleDraw::draw() member.
*/

class QwtScaleDraw: public QwtAbstractScaleDraw
{
public:
    /*!
        Alignment of the scale draw
        \sa setAlignment(), alignment()
     */
    enum Alignment
    {
        //! The scale is below
        BottomScale,

        //! The scale is above
        TopScale,

        //! The scale is left
        LeftScale,

        //! The scale is right
        RightScale
    };

    double Multiplier;

    QwtScaleDraw();
    virtual double extent( const QFont & ) const;

    void move( double x, double y ) { move( QPointF( x, y ) ); }
    void move( const QPointF & );
    void setLength( double length );

    void setAlignment( Alignment );
    Qt::Orientation orientation() const;

    int maxLabelHeight( const QFont & ) const;
    int maxLabelWidth( const QFont & ) const;

    QPointF labelPosition( double val ) const;
    QSizeF labelSize( const QFont &, double val ) const;
    QPointF labelOffset( const QSizeF & ) const;

    virtual void drawTick( QPainter *, double val, double len ) const;
    virtual void drawBackbone( QPainter * ) const;
    virtual void drawLabel( QPainter *, double val ) const;

    void updateMap();

    QPointF pos;
    double len;
    Alignment alignment;
};
