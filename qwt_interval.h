#pragma once

/*!
  \brief A class representing an interval

  The interval is represented by 2 doubles, the lower and the upper limit.
*/

#include <QtGlobal>
class QwtInterval
{
public:
    QwtInterval();
    QwtInterval( double minValue, double maxValue );

    void setInterval( double minValue, double maxValue );

    QwtInterval normalized() const;
    QwtInterval inverted() const;
    QwtInterval limited( double minValue, double maxValue ) const;

    bool operator==( const QwtInterval & ) const;
    bool operator!=( const QwtInterval & ) const;

    double minValue() const;
    double maxValue() const;

    double width() const;

    void setMinValue( double );
    void setMaxValue( double );

    bool contains( double value ) const;

    bool intersects( const QwtInterval & ) const;
    QwtInterval intersect( const QwtInterval & ) const;
    QwtInterval unite( const QwtInterval & ) const;

    QwtInterval operator|( const QwtInterval & ) const;
    QwtInterval operator&( const QwtInterval & ) const;

    QwtInterval &operator|=( const QwtInterval & );
    QwtInterval &operator&=( const QwtInterval & );

    QwtInterval extend( double value ) const;
    QwtInterval operator|( double ) const;
    QwtInterval &operator|=( double );

    bool isValid() const;
    bool isNull() const;
    void invalidate();

    QwtInterval symmetrize( double value ) const;

private:
    double d_minValue;
    double d_maxValue;
};

Q_DECLARE_TYPEINFO(QwtInterval, Q_MOVABLE_TYPE);

/*!
  \brief Default Constructor

  Creates an invalid interval [0.0, -1.0]
  \sa setInterval(), isValid()
*/
inline QwtInterval::QwtInterval():
    d_minValue( 0.0 ),
    d_maxValue( -1.0 )
{
}

/*!
   Constructor

   Build an interval with from min/max values

   \param minValue Minimum value
   \param maxValue Maximum value
*/
inline QwtInterval::QwtInterval(
        double minValue, double maxValue ):
    d_minValue( minValue ),
    d_maxValue( maxValue )
{
}

/*!
   Assign the limits of the interval

   \param minValue Minimum value
   \param maxValue Maximum value
*/
inline void QwtInterval::setInterval(
    double minValue, double maxValue )
{
    d_minValue = minValue;
    d_maxValue = maxValue;
}

/*!
   Assign the lower limit of the interval

   \param minValue Minimum value
*/
inline void QwtInterval::setMinValue( double minValue )
{
    d_minValue = minValue;
}

/*!
   Assign the upper limit of the interval

   \param maxValue Maximum value
*/
inline void QwtInterval::setMaxValue( double maxValue )
{
    d_maxValue = maxValue;
}

//! \return Lower limit of the interval
inline double QwtInterval::minValue() const
{
    return d_minValue;
}

//! \return Upper limit of the interval
inline double QwtInterval::maxValue() const
{
    return d_maxValue;
}

/*!
   Return the width of an interval
   The width of invalid intervals is 0.0, otherwise the result is
   maxValue() - minValue().

   \sa isValid()
*/
inline double QwtInterval::width() const
{
    return isValid() ? ( d_maxValue - d_minValue ) : 0.0;
}

/*!
   Intersection of two intervals
   \sa intersect()
*/
inline QwtInterval QwtInterval::operator&(
    const QwtInterval &interval ) const
{
    return intersect( interval );
}

/*!
   Union of two intervals
   \sa unite()
*/
inline QwtInterval QwtInterval::operator|(
    const QwtInterval &interval ) const
{
    return unite( interval );
}

//! Compare two intervals
inline bool QwtInterval::operator==( const QwtInterval &other ) const
{
    return ( d_minValue == other.d_minValue ) &&
           ( d_maxValue == other.d_maxValue );
}

//! Compare two intervals
inline bool QwtInterval::operator!=( const QwtInterval &other ) const
{
    return ( !( *this == other ) );
}

/*!
   Extend an interval

   \param value Value
   \return Extended interval
   \sa extend()
*/
inline QwtInterval QwtInterval::operator|( double value ) const
{
    return extend( value );
}

//! \return true, if isValid() && (minValue() >= maxValue())
inline bool QwtInterval::isNull() const
{
    return isValid() && d_minValue >= d_maxValue;
}

/*!
   A interval is valid when minValue() <= maxValue().
   In case of QwtInterval::ExcludeBorders it is true
   when minValue() < maxValue()
*/
inline bool QwtInterval::isValid() const
{
    return d_minValue <= d_maxValue;
}

/*!
  Invalidate the interval

  The limits are set to interval [0.0, -1.0]
  \sa isValid()
*/
inline void QwtInterval::invalidate()
{
    d_minValue = 0.0;
    d_maxValue = -1.0;
}
