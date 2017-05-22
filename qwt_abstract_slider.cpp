/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_abstract_slider.h"
#include <qevent.h>

/*!
   \brief Constructor

   \param orientation Orientation
   \param parent Parent widget
*/
QwtAbstractSlider::QwtAbstractSlider(
        Qt::Orientation _orientation, QWidget *parent ):
    QWidget( parent, NULL ),
    scrollMode( false ),
    mouseOffset( 0.0 ),
    tracking( true )
{
    orientation = _orientation;

    setFocusPolicy( Qt::TabFocus );
}

/*!
  \brief Set the orientation.
  \param o Orientation. Allowed values are
           Qt::Horizontal and Qt::Vertical.
*/
void QwtAbstractSlider::setOrientation( Qt::Orientation o )
{
    orientation = o;
}


/*!
   Mouse press event handler
   \param e Mouse event
*/
void QwtAbstractSlider::mousePressEvent( QMouseEvent *e )
{
    const QPoint &p = e->pos();

    if ( (scrollMode = getScrollMode( p )) )
    {
        mouseOffset = getValue( p ) - value;
        Q_EMIT sliderPressed();
    }
}


/*!
   Mouse Release Event handler
   \param e Mouse event
*/
void QwtAbstractSlider::mouseReleaseEvent( QMouseEvent *e )
{
    if ( scrollMode )
    {
        scrollMode = false;

        double prev = value;
        fitValue( getValue( e->pos() ) - mouseOffset );
        Q_EMIT sliderReleased();

        if ( ( !tracking ) || ( value != prev ) )
            Q_EMIT valueChanged( value );
    }
}


/*!
  \brief Enables or disables tracking.

  If tracking is enabled, the slider emits a
  valueChanged() signal whenever its value
  changes (the default behaviour). If tracking
  is disabled, the value changed() signal will only
  be emitted if:<ul>
  <li>the user releases the mouse
      button and the value has changed or
  <li>at the end of automatic scrolling.</ul>
  Tracking is enabled by default.
  \param enable \c true (enable) or \c false (disable) tracking.
*/
void QwtAbstractSlider::setTracking( bool enable )
{
    tracking = enable;
}

/*!
   Mouse Move Event handler
   \param e Mouse event
*/
void QwtAbstractSlider::mouseMoveEvent( QMouseEvent *e )
{
    if ( scrollMode )
    {
        double prev = value;
        fitValue( getValue( e->pos() ) - mouseOffset );
        if ( value != prev )
            Q_EMIT sliderMoved( value );
    }
}

/*!
   Wheel Event handler
   \param e Whell event
*/
void QwtAbstractSlider::wheelEvent( QWheelEvent *e )
{
    // Most mouse types work in steps of 15 degrees, in which case
    // the delta value is a multiple of 120

    double prev = value;
    const int inc = e->delta() / 120;
    incValue( inc );
    if ( value != prev )
        Q_EMIT sliderMoved( value );
}


/*!
  Notify change of value

  This function can be reimplemented by derived classes
  in order to keep track of changes, i.e. repaint the widget.
  The default implementation emits a valueChanged() signal
  if tracking is enabled.
*/
void QwtAbstractSlider::valueChange()
{
    if ( tracking )
        Q_EMIT valueChanged( value );
}


/*!
  \brief Move the slider to a specified value

  This function can be used to move the slider to a value
  which is not an integer multiple of the step size.
  \param val new value
  \sa fitValue()
*/
void QwtAbstractSlider::setValue( double val )
{
    QwtDoubleRange::setValue( val );
}


/*!
  \brief Set the slider's value to the nearest integer multiple
         of the step size.

   \param value Value
   \sa setValue(), incValue()
*/
void QwtAbstractSlider::fitValue( double value )
{
    setNewValue( value, true );
}

/*!
  \brief Increment the value by a specified number of steps
  \param steps number of steps
  \sa setValue()
*/
void QwtAbstractSlider::incValue( int steps )
{
    setNewValue( value + steps * step, true );
}
