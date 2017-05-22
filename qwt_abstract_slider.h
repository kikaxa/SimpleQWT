#pragma once

#include "qwt_double_range.h"
#include <qwidget.h>

/*!
  \brief An abstract base class for slider widgets

  QwtAbstractSlider is a base class for
  slider widgets. It handles mouse events
  and updates the slider's value accordingly. Derived classes
  only have to implement the getValue() and
  getScrollMode() members, and should react to a
  valueChange(), which normally requires repainting.
*/

class QwtAbstractSlider : public QWidget, public QwtDoubleRange
{
    Q_OBJECT

public:
    explicit QwtAbstractSlider( Qt::Orientation, QWidget *parent = NULL );
    //virtual ~QwtAbstractSlider() { }

    void setTracking( bool enable );

    virtual void setOrientation( Qt::Orientation o );

public Q_SLOTS:
    virtual void setValue( double val );
    virtual void fitValue( double val );
    virtual void incValue( int steps );

Q_SIGNALS:

    /*!
      \brief Notify a change of value.

      In the default setting
      (tracking enabled), this signal will be emitted every
      time the value changes ( see setTracking() ).
      \param value new value
    */
    void valueChanged( double value );

    /*!
      This signal is emitted when the user presses the
      movable part of the slider (start ScrMouse Mode).
    */
    void sliderPressed();

    /*!
      This signal is emitted when the user releases the
      movable part of the slider.
    */

    void sliderReleased();
    /*!
      This signal is emitted when the user moves the
      slider with the mouse.
      \param value new value
    */
    void sliderMoved( double value );

protected:
    virtual void valueChange();

    virtual void mousePressEvent( QMouseEvent *e );
    virtual void mouseReleaseEvent( QMouseEvent *e );
    virtual void mouseMoveEvent( QMouseEvent *e );
    virtual void wheelEvent( QWheelEvent *e );

    /*!
      \brief Determine the value corresponding to a specified poind

      This is an abstract virtual function which is called when
      the user presses or releases a mouse button or moves the
      mouse. It has to be implemented by the derived class.
      \param p point
    */
    virtual double getValue( const QPoint & p ) = 0;

    /*!
      \brief Determine what to do when the user presses a mouse button.

      This function is abstract and has to be implemented by derived classes.
      It is called on a mousePress event. The derived class can determine
      what should happen next in dependence of the position where the mouse
      was pressed by returning scrolling mode.

      \param pos point where the mouse was pressed
      \retval scrollMode if enabled
    */
    virtual bool getScrollMode( const QPoint & ) const { return false; }

protected:
    bool scrollMode;
    double mouseOffset;
    int tracking;
    Qt::Orientation orientation;
};
