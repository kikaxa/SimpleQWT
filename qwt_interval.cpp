/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_interval.h"
#include <qalgorithms.h>

/*!
   \brief Normalize the limits of the interval

   If maxValue() < minValue() the limits will be inverted.
   \return Normalized interval

   \sa isValid(), inverted()
*/
QwtInterval QwtInterval::normalized() const
{
    if ( d_minValue > d_maxValue )
        return inverted();

    return *this;
}

/*!
   Invert the limits of the interval
   \return Inverted interval
   \sa normalized()
*/
QwtInterval QwtInterval::inverted() const
{
    return QwtInterval( d_maxValue, d_minValue );
}

/*!
  Test if a value is inside an interval

  \param value Value
  \return true, if value >= minValue() && value <= maxValue()
*/
bool QwtInterval::contains( double value ) const
{
    if ( !isValid() )
        return false;

    if ( value < d_minValue || value > d_maxValue )
        return false;

    return true;
}

//! Unite 2 intervals
QwtInterval QwtInterval::unite( const QwtInterval &other ) const
{
    /*
     If one of the intervals is invalid return the other one.
     If both are invalid return an invalid default interval
     */
    if ( !isValid() )
    {
        if ( !other.isValid() )
            return QwtInterval();
        else
            return other;
    }
    if ( !other.isValid() )
        return *this;

    QwtInterval united;

    // minimum
    if ( d_minValue < other.minValue() )
        united.setMinValue( d_minValue );
    else if ( other.minValue() < d_minValue )
        united.setMinValue( other.minValue() );
    else // d_minValue == other.minValue()
        united.setMinValue( d_minValue );

    // maximum
    if ( d_maxValue > other.maxValue() )
        united.setMaxValue( d_maxValue );
    else if ( other.maxValue() > d_maxValue )
        united.setMaxValue( other.maxValue() );
    else // d_maxValue == other.maxValue() )
        united.setMaxValue( d_maxValue );

    return united;
}

//! Intersect 2 intervals
QwtInterval QwtInterval::intersect( const QwtInterval &other ) const
{
    if ( !other.isValid() || !isValid() )
        return QwtInterval();

    QwtInterval i1 = *this;
    QwtInterval i2 = other;

    // swap i1/i2, so that the minimum of i1
    // is smaller then the minimum of i2

    if ( i1.minValue() > i2.minValue() )
        qSwap( i1, i2 );

    if ( i1.maxValue() < i2.minValue() )
        return QwtInterval();

    QwtInterval intersected;
    intersected.setMinValue( i2.minValue() );

    if ( i1.maxValue() < i2.maxValue() )
        intersected.setMaxValue( i1.maxValue() );
    else if ( i2.maxValue() < i1.maxValue() )
        intersected.setMaxValue( i2.maxValue() );
    else // i1.maxValue() == i2.maxValue()
        intersected.setMaxValue( i1.maxValue() );

    return intersected;
}

//! Unites this interval with the given interval.
QwtInterval& QwtInterval::operator|=( const QwtInterval & interval )
{
    *this = *this | interval;
    return *this;
}

//! Intersects this interval with the given interval.
QwtInterval& QwtInterval::operator&=( const QwtInterval & interval )
{
    *this = *this & interval;
    return *this;
}

/*!
   Test if two intervals overlap
*/
bool QwtInterval::intersects( const QwtInterval &other ) const
{
    if ( !isValid() || !other.isValid() )
        return false;

    QwtInterval i1 = *this;
    QwtInterval i2 = other;

    // swap i1/i2, so that the minimum of i1
    // is smaller then the minimum of i2

    if ( i1.minValue() > i2.minValue() )
        qSwap( i1, i2 );

    if ( i1.maxValue() > i2.minValue() )
        return true;

    if ( i1.maxValue() == i2.minValue() )
        return true;

    return false;
}

/*!
   Adjust the limit that is closer to value, so that value becomes
   the center of the interval.

   \param value Center
   \return Interval with value as center
*/
QwtInterval QwtInterval::symmetrize( double value ) const
{
    if ( !isValid() )
        return *this;

    const double delta =
        qMax( fabs( value - d_maxValue ), fabs( value - d_minValue ) );

    return QwtInterval( value - delta, value + delta );
}

/*!
   Limit the interval, keeping the border modes

   \param lowerBound Lower limit
   \param upperBound Upper limit

   \return Limited interval
*/
QwtInterval QwtInterval::limited( double lowerBound, double upperBound ) const
{
    if ( !isValid() || lowerBound > upperBound )
        return QwtInterval();

    double minValue = qMax( d_minValue, lowerBound );
    minValue = qMin( minValue, upperBound );

    double maxValue = qMax( d_maxValue, lowerBound );
    maxValue = qMin( maxValue, upperBound );

    return QwtInterval( minValue, maxValue );
}

/*!
   Extend the interval

   If value is below minValue, value becomes the lower limit.
   If value is above maxValue, value becomes the upper limit.

   extend has no effect for invalid intervals

   \param value Value
   \sa isValid()
*/
QwtInterval QwtInterval::extend( double value ) const
{
    if ( !isValid() )
        return *this;

    return QwtInterval( qMin( value, d_minValue ),
        qMax( value, d_maxValue ) );
}

/*!
   Extend an interval

   \param value Value
   \return Reference of the extended interval

   \sa extend()
*/
QwtInterval& QwtInterval::operator|=( double value )
{
    *this = *this | value;
    return *this;
}
