/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_scale_engine.h"
#include "qwt_scale_map.h"
#include <qalgorithms.h>
#include <qmath.h>

static const double _eps = 1.0e-6;

/*!
  Ceil a value, relative to an interval

  \param value Value to ceil
  \param intervalSize Interval size

  \sa floorEps()
*/
double QwtScaleArithmetic::ceilEps( double value,
    double intervalSize )
{
    const double eps = _eps * intervalSize;

    value = ( value - eps ) / intervalSize;
    return qwtCeilF( value ) * intervalSize;
}

/*!
  Floor a value, relative to an interval

  \param value Value to floor
  \param intervalSize Interval size

  \sa floorEps()
*/
double QwtScaleArithmetic::floorEps( double value, double intervalSize )
{
    const double eps = _eps * intervalSize;

    value = ( value + eps ) / intervalSize;
    return qwtFloorF( value ) * intervalSize;
}

/*!
  \brief Divide an interval into steps

  \f$stepSize = (intervalSize - intervalSize * 10e^{-6}) / numSteps\f$

  \param intervalSize Interval size
  \param numSteps Number of steps
  \return Step size
*/
double QwtScaleArithmetic::divideEps( double intervalSize, double numSteps )
{
    if ( numSteps == 0.0 || intervalSize == 0.0 )
        return 0.0;

    return ( intervalSize - ( _eps * intervalSize ) ) / numSteps;
}

/*!
  Find the smallest value out of {1,2,5}*10^n with an integer number n
  which is greater than or equal to x

  \param x Input value
*/
double QwtScaleArithmetic::ceil125( double x )
{
    if ( x == 0.0 )
        return 0.0;

    const double sign = ( x > 0 ) ? 1.0 : -1.0;
    const double lx = ::log10( qFabs( x ) );
    const double p10 = qwtFloorF( lx );

    double fr = qPow( 10.0, lx - p10 );
    if ( fr <= 1.0 )
        fr = 1.0;
    else if ( fr <= 2.0 )
        fr = 2.0;
    else if ( fr <= 5.0 )
        fr = 5.0;
    else
        fr = 10.0;

    return sign * fr * qPow( 10.0, p10 );
}


/*!
  Calculate a step size for an interval size

  \param intervalSize Interval size
  \param numSteps Number of steps

  \return Step size
*/
double QwtScaleEngine::divideInterval(
    double intervalSize, int numSteps ) const
{
    if ( numSteps <= 0 )
        return 0.0;

    double v = QwtScaleArithmetic::divideEps( intervalSize, numSteps );
    return QwtScaleArithmetic::ceil125( v );
}

/*!
  Check if an interval "contains" a value

  \param interval Interval
  \param value Value

  \sa QwtScaleArithmetic::compareEps()
*/
bool QwtScaleEngine::contains(
    const QwtInterval &interval, double value ) const
{
    if ( !interval.isValid() )
        return false;

    if ( qwtFuzzyCompare( value, interval.minValue(), interval.width() ) < 0 )
        return false;

    if ( qwtFuzzyCompare( value, interval.maxValue(), interval.width() ) > 0 )
        return false;

    return true;
}

/*!
  Remove ticks from a list, that are not inside an interval

  \param ticks Tick list
  \param interval Interval

  \return Stripped tick list
*/
QList<double> QwtScaleEngine::strip( const QList<double>& ticks,
    const QwtInterval &interval ) const
{
    if ( !interval.isValid() || ticks.count() == 0 )
        return QList<double>();

    if ( contains( interval, ticks.first() )
        && contains( interval, ticks.last() ) )
    {
        return ticks;
    }

    QList<double> strippedTicks;
    for ( int i = 0; i < ( int )ticks.count(); i++ )
    {
        if ( contains( interval, ticks[i] ) )
            strippedTicks += ticks[i];
    }
    return strippedTicks;
}

/*!
  \brief Build an interval for a value

  In case of v == 0.0 the interval is [-0.5, 0.5],
  otherwide it is [0.5 * v, 1.5 * v]
*/

QwtInterval QwtScaleEngine::buildInterval( double v ) const
{
    const double delta = ( v == 0.0 ) ? 0.5 : fabs( 0.5 * v );
    return QwtInterval( v - delta, v + delta );
}

/*!
  Return a transformation, for linear scales
*/
QwtScaleTransformation *QwtLinearScaleEngine::transformation() const
{
    return new QwtScaleTransformation( QwtScaleTransformation::Linear );
}

/*!
   \brief Calculate a scale division

   \param x1 First interval limit
   \param x2 Second interval limit
   \param maxMajSteps Maximum for the number of major steps
   \param maxMinSteps Maximum number of minor steps
   \param stepSize Step size. If stepSize == 0, the scaleEngine
                   calculates one.

   \sa QwtScaleEngine::stepSize(), QwtScaleEngine::subDivide()
*/
QwtScaleDiv QwtLinearScaleEngine::divideScale( double x1, double x2,
    int maxMajSteps, int maxMinSteps, double stepSize ) const
{
    QwtInterval interval = QwtInterval( x1, x2 ).normalized();
    if ( interval.width() <= 0 )
        return QwtScaleDiv();

    stepSize = fabs( stepSize );
    if ( stepSize == 0.0 )
    {
        if ( maxMajSteps < 1 )
            maxMajSteps = 1;

        stepSize = divideInterval( interval.width(), maxMajSteps );
    }

    QwtScaleDiv scaleDiv;

    if ( stepSize != 0.0 )
    {
        QList<double> ticks[QwtScaleDiv::NTickTypes];
        buildTicks( interval, stepSize, maxMinSteps, ticks );

        scaleDiv = QwtScaleDiv( interval, ticks );
    }

    if ( x1 > x2 )
        scaleDiv.invert();

    return scaleDiv;
}

/*!
   \brief Calculate ticks for an interval

   \param interval Interval
   \param stepSize Step size
   \param maxMinSteps Maximum number of minor steps
   \param ticks Arrays to be filled with the calculated ticks

   \sa buildMajorTicks(), buildMinorTicks
*/
void QwtLinearScaleEngine::buildTicks(
    const QwtInterval& interval, double stepSize, int maxMinSteps,
    QList<double> ticks[QwtScaleDiv::NTickTypes] ) const
{
    const QwtInterval boundingInterval =
        align( interval, stepSize );

    ticks[QwtScaleDiv::MajorTick] =
        buildMajorTicks( boundingInterval, stepSize );

    if ( maxMinSteps > 0 )
    {
        buildMinorTicks( ticks[QwtScaleDiv::MajorTick], maxMinSteps, stepSize,
            ticks[QwtScaleDiv::MinorTick], ticks[QwtScaleDiv::MediumTick] );
    }

    for ( int i = 0; i < QwtScaleDiv::NTickTypes; i++ )
    {
        ticks[i] = strip( ticks[i], interval );

        // ticks very close to 0.0 are
        // explicitely set to 0.0

        for ( int j = 0; j < ( int )ticks[i].count(); j++ )
        {
            if ( qwtFuzzyCompare( ticks[i][j], 0.0, stepSize ) == 0 )
                ticks[i][j] = 0.0;
        }
    }
}

/*!
   \brief Calculate major ticks for an interval

   \param interval Interval
   \param stepSize Step size

   \return Calculated ticks
*/
QList<double> QwtLinearScaleEngine::buildMajorTicks(
    const QwtInterval &interval, double stepSize ) const
{
    int numTicks = qRound( interval.width() / stepSize ) + 1;
    if ( numTicks > 10000 )
        numTicks = 10000;

    QList<double> ticks;

    ticks += interval.minValue();
    for ( int i = 1; i < numTicks - 1; i++ )
        ticks += interval.minValue() + i * stepSize;
    ticks += interval.maxValue();

    return ticks;
}

/*!
   \brief Calculate minor/medium ticks for major ticks

   \param majorTicks Major ticks
   \param maxMinSteps Maximum number of minor steps
   \param stepSize Step size
   \param minorTicks Array to be filled with the calculated minor ticks
   \param mediumTicks Array to be filled with the calculated medium ticks

*/
void QwtLinearScaleEngine::buildMinorTicks(
    const QList<double>& majorTicks,
    int maxMinSteps, double stepSize,
    QList<double> &minorTicks,
    QList<double> &mediumTicks ) const
{
    double minStep = divideInterval( stepSize, maxMinSteps );
    if ( minStep == 0.0 )
        return;

    // # ticks per interval
    int numTicks = qCeil( fabs( stepSize / minStep ) ) - 1;

    // Do the minor steps fit into the interval?
    if ( qwtFuzzyCompare( ( numTicks +  1 ) * fabs( minStep ),
        fabs( stepSize ), stepSize ) > 0 )
    {
        numTicks = 1;
        minStep = stepSize * 0.5;
    }

    int medIndex = -1;
    if ( numTicks % 2 )
        medIndex = numTicks / 2;

    // calculate minor ticks

    for ( int i = 0; i < ( int )majorTicks.count(); i++ )
    {
        double val = majorTicks[i];
        for ( int k = 0; k < numTicks; k++ )
        {
            val += minStep;

            double alignedValue = val;
            if ( qwtFuzzyCompare( val, 0.0, stepSize ) == 0 )
                alignedValue = 0.0;

            if ( k == medIndex )
                mediumTicks += alignedValue;
            else
                minorTicks += alignedValue;
        }
    }
}

/*!
  \brief Align an interval to a step size

  The limits of an interval are aligned that both are integer
  multiples of the step size.

  \param interval Interval
  \param stepSize Step size

  \return Aligned interval
*/
QwtInterval QwtLinearScaleEngine::align(
    const QwtInterval &interval, double stepSize ) const
{
    double x1 = QwtScaleArithmetic::floorEps( interval.minValue(), stepSize );
    if ( qwtFuzzyCompare( interval.minValue(), x1, stepSize ) == 0 )
        x1 = interval.minValue();

    double x2 = QwtScaleArithmetic::ceilEps( interval.maxValue(), stepSize );
    if ( qwtFuzzyCompare( interval.maxValue(), x2, stepSize ) == 0 )
        x2 = interval.maxValue();

    return QwtInterval( x1, x2 );
}

/*!
  Return a transformation, for logarithmic (base 10) scales
*/
QwtScaleTransformation *QwtLog10ScaleEngine::transformation() const
{
    return new QwtScaleTransformation( QwtScaleTransformation::Log10 );
}

/*!
   \brief Calculate a scale division

   \param x1 First interval limit
   \param x2 Second interval limit
   \param maxMajSteps Maximum for the number of major steps
   \param maxMinSteps Maximum number of minor steps
   \param stepSize Step size. If stepSize == 0, the scaleEngine
                   calculates one.

   \sa QwtScaleEngine::stepSize(), QwtLog10ScaleEngine::subDivide()
*/
QwtScaleDiv QwtLog10ScaleEngine::divideScale( double x1, double x2,
    int maxMajSteps, int maxMinSteps, double stepSize ) const
{
    QwtInterval interval = QwtInterval( x1, x2 ).normalized();
    interval = interval.limited( LOG_MIN, LOG_MAX );

    if ( interval.width() <= 0 )
        return QwtScaleDiv();

    if ( interval.maxValue() / interval.minValue() < 10.0 )
    {
        // scale width is less than one decade -> build linear scale

        QwtLinearScaleEngine linearScaler;

        if ( stepSize != 0.0 )
            stepSize = qPow( 10.0, stepSize );

        return linearScaler.divideScale( x1, x2,
            maxMajSteps, maxMinSteps, stepSize );
    }

    stepSize = fabs( stepSize );
    if ( stepSize == 0.0 )
    {
        if ( maxMajSteps < 1 )
            maxMajSteps = 1;

        stepSize = divideInterval( log10( interval ).width(), maxMajSteps );
        if ( stepSize < 1.0 )
            stepSize = 1.0; // major step must be >= 1 decade
    }

    QwtScaleDiv scaleDiv;
    if ( stepSize != 0.0 )
    {
        QList<double> ticks[QwtScaleDiv::NTickTypes];
        buildTicks( interval, stepSize, maxMinSteps, ticks );

        scaleDiv = QwtScaleDiv( interval, ticks );
    }

    if ( x1 > x2 )
        scaleDiv.invert();

    return scaleDiv;
}

/*!
   \brief Calculate ticks for an interval

   \param interval Interval
   \param maxMinSteps Maximum number of minor steps
   \param stepSize Step size
   \param ticks Arrays to be filled with the calculated ticks

   \sa buildMajorTicks(), buildMinorTicks
*/
void QwtLog10ScaleEngine::buildTicks(
    const QwtInterval& interval, double stepSize, int maxMinSteps,
    QList<double> ticks[QwtScaleDiv::NTickTypes] ) const
{
    const QwtInterval boundingInterval = align( interval, stepSize );

    ticks[QwtScaleDiv::MajorTick] =
        buildMajorTicks( boundingInterval, stepSize );

    if ( maxMinSteps > 0 )
    {
        ticks[QwtScaleDiv::MinorTick] = buildMinorTicks(
            ticks[QwtScaleDiv::MajorTick], maxMinSteps, stepSize );
    }

    for ( int i = 0; i < QwtScaleDiv::NTickTypes; i++ )
        ticks[i] = strip( ticks[i], interval );
}

/*!
   \brief Calculate major ticks for an interval

   \param interval Interval
   \param stepSize Step size

   \return Calculated ticks
*/
QList<double> QwtLog10ScaleEngine::buildMajorTicks(
    const QwtInterval &interval, double stepSize ) const
{
    double width = log10( interval ).width();

    int numTicks = qRound( width / stepSize ) + 1;
    if ( numTicks > 10000 )
        numTicks = 10000;

    const double lxmin = ::log( interval.minValue() );
    const double lxmax = ::log( interval.maxValue() );
    const double lstep = ( lxmax - lxmin ) / double( numTicks - 1 );

    QList<double> ticks;

    ticks += interval.minValue();

    for ( int i = 1; i < numTicks - 1; i++ )
        ticks += qExp( lxmin + double( i ) * lstep );

    ticks += interval.maxValue();

    return ticks;
}

/*!
   \brief Calculate minor/medium ticks for major ticks

   \param majorTicks Major ticks
   \param maxMinSteps Maximum number of minor steps
   \param stepSize Step size
*/
QList<double> QwtLog10ScaleEngine::buildMinorTicks(
    const QList<double> &majorTicks,
    int maxMinSteps, double stepSize ) const
{
    if ( stepSize < 1.1 )          // major step width is one decade
    {
        if ( maxMinSteps < 1 )
            return QList<double>();

        int k0, kstep, kmax;

        if ( maxMinSteps >= 8 )
        {
            k0 = 2;
            kmax = 9;
            kstep = 1;
        }
        else if ( maxMinSteps >= 4 )
        {
            k0 = 2;
            kmax = 8;
            kstep = 2;
        }
        else if ( maxMinSteps >= 2 )
        {
            k0 = 2;
            kmax = 5;
            kstep = 3;
        }
        else
        {
            k0 = 5;
            kmax = 5;
            kstep = 1;
        }

        QList<double> minorTicks;

        for ( int i = 0; i < ( int )majorTicks.count(); i++ )
        {
            const double v = majorTicks[i];
            for ( int k = k0; k <= kmax; k += kstep )
                minorTicks += v * double( k );
        }

        return minorTicks;
    }
    else  // major step > one decade
    {
        double minStep = divideInterval( stepSize, maxMinSteps );
        if ( minStep == 0.0 )
            return QList<double>();

        if ( minStep < 1.0 )
            minStep = 1.0;

        // # subticks per interval
        int nMin = qRound( stepSize / minStep ) - 1;

        // Do the minor steps fit into the interval?

        if ( qwtFuzzyCompare( ( nMin +  1 ) * minStep,
            fabs( stepSize ), stepSize ) > 0 )
        {
            nMin = 0;
        }

        if ( nMin < 1 )
            return QList<double>();      // no subticks

        // substep factor = 10^substeps
        const qreal minFactor = qMax( qPow( 10.0, minStep ), qreal( 10.0 ) );

        QList<double> minorTicks;
        for ( int i = 0; i < ( int )majorTicks.count(); i++ )
        {
            double val = majorTicks[i];
            for ( int k = 0; k < nMin; k++ )
            {
                val *= minFactor;
                minorTicks += val;
            }
        }
        return minorTicks;
    }
}

/*!
  \brief Align an interval to a step size

  The limits of an interval are aligned that both are integer
  multiples of the step size.

  \param interval Interval
  \param stepSize Step size

  \return Aligned interval
*/
QwtInterval QwtLog10ScaleEngine::align(
    const QwtInterval &interval, double stepSize ) const
{
    const QwtInterval intv = log10( interval );

    double x1 = QwtScaleArithmetic::floorEps( intv.minValue(), stepSize );
    if ( qwtFuzzyCompare( interval.minValue(), x1, stepSize ) == 0 )
        x1 = interval.minValue();

    double x2 = QwtScaleArithmetic::ceilEps( intv.maxValue(), stepSize );
    if ( qwtFuzzyCompare( interval.maxValue(), x2, stepSize ) == 0 )
        x2 = interval.maxValue();

    return pow10( QwtInterval( x1, x2 ) );
}

/*!
  Return the interval [log10(interval.minValue(), log10(interval.maxValue]
*/

QwtInterval QwtLog10ScaleEngine::log10( const QwtInterval &interval ) const
{
    return QwtInterval( ::log10( interval.minValue() ),
            ::log10( interval.maxValue() ) );
}

/*!
  Return the interval [pow10(interval.minValue(), pow10(interval.maxValue]
*/
QwtInterval QwtLog10ScaleEngine::pow10( const QwtInterval &interval ) const
{
    return QwtInterval( qPow( 10.0, interval.minValue() ),
            qPow( 10.0, interval.maxValue() ) );
}
