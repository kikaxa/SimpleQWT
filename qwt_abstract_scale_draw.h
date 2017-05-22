#pragma once

#include "qwt_scale_div.h"
#include "qwt_text.h"

class QPalette;
class QPainter;
class QFont;
class QwtScaleTransformation;
class QwtScaleMap;

/*!
  \brief A abstract base class for drawing scales

  QwtAbstractScaleDraw can be used to draw linear or logarithmic scales.

  After a scale division has been specified as a QwtScaleDiv object
  using QwtAbstractScaleDraw::setScaleDiv(const QwtScaleDiv &s),
  the scale can be drawn with the QwtAbstractScaleDraw::draw() member.
*/
class QwtAbstractScaleDraw
{
public:
    QwtAbstractScaleDraw();
    virtual ~QwtAbstractScaleDraw();

    void setScaleDiv( const QwtScaleDiv &s );
    const QwtScaleDiv& scaleDiv() const;

    void setTransformation( QwtScaleTransformation * );
    const QwtScaleMap &scaleMap() const;
    QwtScaleMap &scaleMap();

    void setTickLength( QwtScaleDiv::TickType, double length );
    double tickLength( QwtScaleDiv::TickType ) const;
    double maxTickLength() const;

    void setSpacing( double margin );
    double spacing() const;

    void setPenWidth( int width );
    int penWidth() const;

    virtual void draw( QPainter *, const QPalette & ) const;

    virtual QwtText label( double ) const;

    /*!
      Calculate the extent

      The extent is the distcance from the baseline to the outermost
      pixel of the scale draw in opposite to its orientation.
      It is at least minimumExtent() pixels.

      \sa setMinimumExtent(), minimumExtent()
    */
    virtual double extent( const QFont & ) const = 0;

protected:
    /*!
       Draw a tick

       \param painter Painter
       \param value Value of the tick
       \param len Lenght of the tick

       \sa drawBackbone(), drawLabel()
    */
    virtual void drawTick( QPainter *painter, double value, double len ) const = 0;

    /*!
      Draws the baseline of the scale
      \param painter Painter

      \sa drawTick(), drawLabel()
    */
    virtual void drawBackbone( QPainter *painter ) const = 0;

    /*!
        Draws the label for a major scale tick

        \param painter Painter
        \param value Value

        \sa drawTick(), drawBackbone()
    */
    virtual void drawLabel( QPainter *painter, double value ) const = 0;

    void invalidateCache();
    const QwtText &tickLabel( double value ) const;

private:
    QwtAbstractScaleDraw( const QwtAbstractScaleDraw & );
    QwtAbstractScaleDraw &operator=( const QwtAbstractScaleDraw & );

    class PrivateData;
    mutable PrivateData *d_data;
};
