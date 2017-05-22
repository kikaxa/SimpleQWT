#pragma once

#include "qwt_math.h"

class QRectF;

/*!
   \brief Operations for linear or logarithmic (base 10) transformations
*/
class QwtScaleTransformation
{
public:
    //! Transformation type
    enum Type
    {
        //! Transformation between 2 linear scales
        Linear,

        //! Transformation between a linear and a logarithmic ( base 10 ) scale
        Log10,

        //! Any other type of transformation
        Other
    };

    QwtScaleTransformation( Type type );
    virtual ~QwtScaleTransformation();

    virtual double xForm( double x, double s1, double s2,
        double p1, double p2 ) const;
    virtual double invXForm( double x, double s1, double s2,
        double p1, double p2 ) const;

    Type type() const;

    virtual QwtScaleTransformation *copy() const;

private:
    QwtScaleTransformation();
    QwtScaleTransformation &operator=( const QwtScaleTransformation );

    const Type d_type;
};

//! \return Transformation type
inline QwtScaleTransformation::Type QwtScaleTransformation::type() const
{
    return d_type;
}

/*!
   \brief A scale map

   QwtScaleMap offers transformations from a scale
   into a paint interval and vice versa.
*/
class QwtScaleMap
{
public:
    QwtScaleMap();
    QwtScaleMap( const QwtScaleMap& );

    ~QwtScaleMap();

    QwtScaleMap &operator=( const QwtScaleMap & );

    void setTransformation( QwtScaleTransformation * );
    const QwtScaleTransformation *transformation() const;

    void setPaintInterval( double p1, double p2 );
    void setScaleInterval( double s1, double s2 );

    double transform( double s ) const;
    double invTransform( double p ) const;

    double p1() const;
    double p2() const;

    double s1() const;
    double s2() const;

    double pDist() const;
    double sDist() const;

    static double LogMin;
    static double LogMax;

    static QRectF transform( const QwtScaleMap &,
        const QwtScaleMap &, const QRectF & );
    static QRectF invTransform( const QwtScaleMap &,
        const QwtScaleMap &, const QRectF & );

    static QPointF transform( const QwtScaleMap &,
        const QwtScaleMap &, const QPointF & );
    static QPointF invTransform( const QwtScaleMap &,
        const QwtScaleMap &, const QPointF & );

    bool isInverting() const;

private:
    void newFactor();

    double d_s1, d_s2;     // scale interval boundaries
    double d_p1, d_p2;     // paint device interval boundaries

    double d_cnv;       // conversion factor

    QwtScaleTransformation *d_transformation;
};

/*!
    \return First border of the scale interval
*/
inline double QwtScaleMap::s1() const
{
    return d_s1;
}

/*!
    \return Second border of the scale interval
*/
inline double QwtScaleMap::s2() const
{
    return d_s2;
}

/*!
    \return First border of the paint interval
*/
inline double QwtScaleMap::p1() const
{
    return d_p1;
}

/*!
    \return Second border of the paint interval
*/
inline double QwtScaleMap::p2() const
{
    return d_p2;
}

/*!
    \return qwtAbs(p2() - p1())
*/
inline double QwtScaleMap::pDist() const
{
    return fabs( d_p2 - d_p1 );
}

/*!
    \return qwtAbs(s2() - s1())
*/
inline double QwtScaleMap::sDist() const
{
    return fabs( d_s2 - d_s1 );
}

/*!
  Transform a point related to the scale interval into an point
  related to the interval of the paint device

  \param s Value relative to the coordinates of the scale
*/
inline double QwtScaleMap::transform( double s ) const
{
    // try to inline code from QwtScaleTransformation

    if ( d_transformation->type() == QwtScaleTransformation::Linear )
        return d_p1 + ( s - d_s1 ) * d_cnv;

    if ( d_transformation->type() == QwtScaleTransformation::Log10 )
        return d_p1 + log( s / d_s1 ) * d_cnv;

    return d_transformation->xForm( s, d_s1, d_s2, d_p1, d_p2 );
}

/*!
  Transform an paint device value into a value in the
  interval of the scale.

  \param p Value relative to the coordinates of the paint device
  \sa transform()
*/
inline double QwtScaleMap::invTransform( double p ) const
{
    return d_transformation->invXForm( p, d_p1, d_p2, d_s1, d_s2 );
}

//! \return True, when ( p1() < p2() ) != ( s1() < s2() )
inline bool QwtScaleMap::isInverting() const
{
    return ( ( d_p1 < d_p2 ) != ( d_s1 < d_s2 ) );
}
