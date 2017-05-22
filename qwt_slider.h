#pragma once

#include "qwt_abstract_scale.h"
#include "qwt_abstract_slider.h"

class QwtScaleDraw;

/*!
  \brief The Slider Widget

  QwtSlider is a slider widget which operates on an interval
  of type double. QwtSlider supports different layouts as
  well as a scale.

  \image html sliders.png

  \sa QwtAbstractSlider and QwtAbstractScale for the descriptions
      of the inherited members.
*/

class QwtSlider : public QwtAbstractSlider, public QwtAbstractScale
{
    Q_OBJECT

public:

    /*!
      Scale position. QwtSlider tries to enforce valid combinations of its
      orientation and scale position:

      - Qt::Horizonal combines with NoScale, TopScale and BottomScale
      - Qt::Vertical combines with NoScale, LeftScale and RightScale

      \sa QwtSlider()
     */
    enum ScalePos
    {
        //! The slider has no scale
        NoScale,

        //! The scale is left of the slider
        LeftScale,

        //! The scale is right of the slider
        RightScale,

        //! The scale is above of the slider
        TopScale,

        //! The scale is below of the slider
        BottomScale
    };

    explicit QwtSlider( QWidget *parent,
        Qt::Orientation = Qt::Horizontal,
        ScalePos = NoScale);

    virtual ~QwtSlider();

    virtual void setOrientation( Qt::Orientation );

    void setScalePosition( ScalePos s );
    ScalePos scalePosition() const;

    void setHandleSize( int width, int height );
    void setHandleSize( const QSize & );
    QSize handleSize() const;

    void setBorderWidth( int bw );
    int borderWidth() const;

    void setSpacing( int );
    int spacing() const;

    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;

    void setScaleDraw( QwtScaleDraw * );

protected:
    virtual double getValue( const QPoint &p );
    virtual bool getScrollMode( const QPoint &p ) const;

    virtual void drawSlider ( QPainter *, const QRect & ) const;
    virtual void drawHandle( QPainter *, const QRect &, int pos ) const;

    virtual void resizeEvent( QResizeEvent * );
    virtual void paintEvent ( QPaintEvent * );
    virtual void changeEvent( QEvent * );

    virtual void valueChange();
    virtual void rangeChange();
    virtual void scaleChange();

    int transform( double v ) const;

private:
    void layoutSlider( bool );
    void initSlider( Qt::Orientation, ScalePos );

    class PrivateData;
    PrivateData *d_data;
};
