/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_double_range.h"
#include "qmath.h"

QwtDoubleRange::QwtDoubleRange():
    minValue( 0.0 ),
    maxValue( 0.0 ),
    step( 1.0 ),
    value( 0.0 )
{
}

void QwtDoubleRange::setNewValue( double _value, bool align )
{
    double prevValue = value;

    const double vmin = qMin( minValue, maxValue );
    const double vmax = qMax( minValue, maxValue );

    value = qBound(vmin, _value, vmax);

    if ( align )
    {
        value = minValue +
            qRound( ( value - minValue ) / step ) * step;

        const double minEps = 1.0e-10;
        // correct rounding error at the border
        if ( qFabs( value - maxValue ) < minEps * fabs( step ) )
            value = maxValue;

        // correct rounding error if value = 0
        if ( qFabs( value ) < minEps * fabs( step ) )
            value = 0.0;
    }

    if ( prevValue != value )
    {
        valueChange();
    }
}


/*!
  \brief Set a new value without adjusting to the step raster
  \param x new value
  \warning The value is clipped when it lies outside the range.
*/
void QwtDoubleRange::setValue( double x )
{
    setNewValue( x, false );
}

/*!
  \brief Specify  range and step size

  \param vmin   lower boundary of the interval
  \param vmax   higher boundary of the interval
  \param vstep  step width
  \warning
  \li A change of the range changes the value if it lies outside the
      new range. The current value
      will *not* be adjusted to the new step raster.
  \li vmax < vmin is allowed.
  \li If the step size is left out or set to zero, it will be
      set to 1/100 of the interval length.
*/
void QwtDoubleRange::setRange( 
    double vmin, double vmax, double vstep )
{
    const bool rchg = ( maxValue != vmax || minValue != vmin );

    if ( rchg )
    {
        minValue = vmin;
        maxValue = vmax;
    }

    setStep( vstep );

    // If the value lies out of the range, it
    // will be changed. Note that it will not be adjusted to
    // the new step width.
    setNewValue( value, false );

    if ( rchg )
        rangeChange();
}

/*!
  \brief Change the step raster
  \param vstep new step width
  \warning The value will \e not be adjusted to the new step raster.
*/
void QwtDoubleRange::setStep( double vstep )
{
    const double intv = maxValue - minValue;

    double newStep = qFabs(vstep);
    if ( newStep < qFabs( 1e-10 * intv ) )
        newStep = qFabs( intv * 1e-2 );

    step = newStep;
}
