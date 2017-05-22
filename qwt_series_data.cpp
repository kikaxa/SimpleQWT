/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_series_data.h"

static inline QRectF qwtBoundingRect( const QPointF &sample )
{
    return QRectF( sample.x(), sample.y(), 0.0, 0.0 );
}

/*!
  \brief Calculate the bounding rect of a series subset

  Slow implementation, that iterates over the series.

  \param series Series
  \param from Index of the first sample, <= 0 means from the beginning
  \param to Index of the last sample, < 0 means to the end

  \return Bounding rectangle
*/

template <class T>
QRectF qwtBoundingRectT( 
    const QwtSeriesData<T>& series, int from, int to )
{
    QRectF boundingRect( 1.0, 1.0, -2.0, -2.0 ); // invalid;

    if ( from < 0 )
        from = 0;

    if ( to < 0 )
        to = series.size() - 1;

    if ( to < from )
        return boundingRect;

    int i;
    for ( i = from; i <= to; i++ )
    {
        const QRectF rect = qwtBoundingRect( series.sample( i ) );
        if ( rect.width() >= 0.0 && rect.height() >= 0.0 )
        {
            boundingRect = rect;
            i++;
            break;
        }
    }

    for ( ; i <= to; i++ )
    {
        const QRectF rect = qwtBoundingRect( series.sample( i ) );
        if ( rect.width() >= 0.0 && rect.height() >= 0.0 )
        {
            boundingRect.setLeft( qMin( boundingRect.left(), rect.left() ) );
            boundingRect.setRight( qMax( boundingRect.right(), rect.right() ) );
            boundingRect.setTop( qMin( boundingRect.top(), rect.top() ) );
            boundingRect.setBottom( qMax( boundingRect.bottom(), rect.bottom() ) );
        }
    }

    return boundingRect;
}

/*!
  \brief Calculate the bounding rect of a series subset

  Slow implementation, that iterates over the series.

  \param series Series
  \param from Index of the first sample, <= 0 means from the beginning
  \param to Index of the last sample, < 0 means to the end

  \return Bounding rectangle
*/
QRectF qwtBoundingRect( 
    const QwtSeriesData<QPointF> &series, int from, int to )
{
    return qwtBoundingRectT<QPointF>( series, from, to );
}

/*!
   Constructor
   \param samples Samples
*/
QwtPointSeriesData::QwtPointSeriesData(
        const QVector<QPointF> &samples ):
    QwtArraySeriesData<QPointF>( samples )
{
}

/*!
  \brief Calculate the bounding rect

  The bounding rectangle is calculated once by iterating over all
  points and is stored for all following requests.

  \return Bounding rectangle
*/
QRectF QwtPointSeriesData::boundingRect() const
{
    if ( d_boundingRect.width() < 0.0 )
        d_boundingRect = qwtBoundingRect( *this );

    return d_boundingRect;
}


/*!
  Constructor

  \param x Array of x values
  \param y Array of y values

  \sa QwtPlotCurve::setData(), QwtPlotCurve::setSamples()
*/
QwtPointArrayData::QwtPointArrayData(
        const QVector<double> &x, const QVector<double> &y ):
    d_x( x ),
    d_y( y )
{
}

/*!
  Constructor

  \param x Array of x values
  \param y Array of y values
  \param size Size of the x and y arrays
  \sa QwtPlotCurve::setData(), QwtPlotCurve::setSamples()
*/
QwtPointArrayData::QwtPointArrayData( const double *x,
        const double *y, int size )
{
    d_x.resize( size );
    memcpy( d_x.data(), x, size * sizeof( double ) );

    d_y.resize( size );
    memcpy( d_y.data(), y, size * sizeof( double ) );
}

/*!
  \brief Calculate the bounding rect

  The bounding rectangle is calculated once by iterating over all
  points and is stored for all following requests.

  \return Bounding rectangle
*/
QRectF QwtPointArrayData::boundingRect() const
{
    if ( d_boundingRect.width() < 0 )
        d_boundingRect = qwtBoundingRect( *this );

    return d_boundingRect;
}

//! \return Size of the data set
int QwtPointArrayData::size() const
{
    return qMin( d_x.size(), d_y.size() );
}

/*!
  Return the sample at position i

  \param i Index
  \return Sample at position i
*/
QPointF QwtPointArrayData::sample( int i ) const
{
    return QPointF( d_x[int( i )], d_y[int( i )] );
}

//! \return Array of the x-values
const QVector<double> &QwtPointArrayData::xData() const
{
    return d_x;
}

//! \return Array of the y-values
const QVector<double> &QwtPointArrayData::yData() const
{
    return d_y;
}

/*!
  Constructor

  \param x Array of x values
  \param y Array of y values
  \param size Size of the x and y arrays

  \warning The programmer must assure that the memory blocks referenced
           by the pointers remain valid during the lifetime of the
           QwtPlotCPointer object.

  \sa QwtPlotCurve::setData(), QwtPlotCurve::setRawSamples()
*/
QwtCPointerData::QwtCPointerData(
        const double *x, const double *y, int size ):
    d_x( x ),
    d_y( y ),
    d_size( size )
{
}

/*!
  \brief Calculate the bounding rect

  The bounding rectangle is calculated once by iterating over all
  points and is stored for all following requests.

  \return Bounding rectangle
*/
QRectF QwtCPointerData::boundingRect() const
{
    if ( d_boundingRect.width() < 0 )
        d_boundingRect = qwtBoundingRect( *this );

    return d_boundingRect;
}

//! \return Size of the data set
int QwtCPointerData::size() const
{
    return d_size;
}

/*!
  Return the sample at position i

  \param i Index
  \return Sample at position i
*/
QPointF QwtCPointerData::sample( int i ) const
{
    return QPointF( d_x[int( i )], d_y[int( i )] );
}

//! \return Array of the x-values
const double *QwtCPointerData::xData() const
{
    return d_x;
}

//! \return Array of the y-values
const double *QwtCPointerData::yData() const
{
    return d_y;
}
