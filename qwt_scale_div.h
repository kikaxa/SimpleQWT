#pragma once

#include "qwt_interval.h"
#include <qlist.h>

class QwtInterval;

/*!
  \brief A class representing a scale division

  A scale division consists of its limits and 3 list
  of tick values qualified as major, medium and minor ticks.

  In most cases scale divisions are calculated by a QwtScaleEngine.

  \sa subDivideInto(), subDivide()
*/

class QwtScaleDiv
{
public:
    //! Scale tick types
    enum TickType
    {
        //! No ticks
        NoTick = -1,

        //! Minor ticks
        MinorTick,

        //! Medium ticks
        MediumTick,

        //! Major ticks
        MajorTick,

        //! Number of valid tick types
        NTickTypes
    };

    explicit QwtScaleDiv();
    explicit QwtScaleDiv( const QwtInterval &, QList<double>[NTickTypes] );
    explicit QwtScaleDiv( 
        double lowerBound, double upperBound, QList<double>[NTickTypes] );

    bool operator==( const QwtScaleDiv &s ) const;
    bool operator!=( const QwtScaleDiv &s ) const;

    void setInterval( double lowerBound, double upperBound );
    void setInterval( const QwtInterval & );
    QwtInterval interval() const;

    double lowerBound() const;
    double upperBound() const;
    double range() const;

    void setTicks( int type, const QList<double> & );
    const QList<double> &ticks( int type ) const;

    void invalidate();
    bool isValid() const;

    void invert();

private:
    double d_lowerBound;
    double d_upperBound;
    QList<double> d_ticks[NTickTypes];

    bool d_isValid;
};

Q_DECLARE_TYPEINFO(QwtScaleDiv, Q_MOVABLE_TYPE);

/*!
   Change the interval
   \param lowerBound lower bound
   \param upperBound upper bound
*/
inline void QwtScaleDiv::setInterval( double lowerBound, double upperBound )
{
    d_lowerBound = lowerBound;
    d_upperBound = upperBound;
}

/*!
  \return lowerBound -> upperBound
*/
inline QwtInterval QwtScaleDiv::interval() const
{
    return QwtInterval( d_lowerBound, d_upperBound );
}

/*!
  \return lower bound
  \sa upperBound()
*/
inline double QwtScaleDiv::lowerBound() const
{
    return d_lowerBound;
}

/*!
  \return upper bound
  \sa lowerBound()
*/
inline double QwtScaleDiv::upperBound() const
{
    return d_upperBound;
}

/*!
  \return upperBound() - lowerBound()
*/
inline double QwtScaleDiv::range() const
{
    return d_upperBound - d_lowerBound;
}
