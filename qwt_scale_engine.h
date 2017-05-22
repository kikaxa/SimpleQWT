#pragma once

#include "qwt_scale_div.h"
#include "qwt_interval.h"

class QwtScaleTransformation;

/*!
  \brief Arithmetic including a tolerance
*/
class QwtScaleArithmetic
{
public:
    static double ceilEps( double value, double intervalSize );
    static double floorEps( double value, double intervalSize );

    static double divideEps( double interval, double steps );

    static double ceil125( double x );
};

/*!
  \brief Base class for scale engines.

  A scale engine trys to find "reasonable" ranges and step sizes
  for scales.

  Qwt offers implementations for logarithmic (log10)
  and linear scales. Contributions for other types of scale engines
  (date/time, log2 ... ) are welcome.
*/

class QwtScaleEngine
{
public:
    virtual ~QwtScaleEngine() { }

    /*!
      \brief Calculate a scale division

      \param x1 First interval limit
      \param x2 Second interval limit
      \param maxMajSteps Maximum for the number of major steps
      \param maxMinSteps Maximum number of minor steps
      \param stepSize Step size. If stepSize == 0.0, the scaleEngine
                   calculates one.
    */
    virtual QwtScaleDiv divideScale( double x1, double x2,
        int maxMajSteps, int maxMinSteps,
        double stepSize = 0.0 ) const = 0;

    //! \return a transformation
    virtual QwtScaleTransformation *transformation() const = 0;

protected:
    bool contains( const QwtInterval &, double val ) const;
    QList<double> strip( const QList<double>&, const QwtInterval & ) const;
    double divideInterval( double interval, int numSteps ) const;

    QwtInterval buildInterval( double v ) const;
};

/*!
  \brief A scale engine for linear scales

  The step size will fit into the pattern
  \f$\left\{ 1,2,5\right\} \cdot 10^{n}\f$, where n is an integer.
*/

class QwtLinearScaleEngine: public QwtScaleEngine
{
public:
    virtual QwtScaleDiv divideScale( double x1, double x2,
        int numMajorSteps, int numMinorSteps,
                                     double stepSize = 0.0 ) const;

    virtual QwtScaleTransformation *transformation() const;

protected:
    QwtInterval align( const QwtInterval&, double stepSize ) const;

    void buildTicks(
        const QwtInterval &, double stepSize, int maxMinSteps,
        QList<double> ticks[QwtScaleDiv::NTickTypes] ) const;

    QList<double> buildMajorTicks(
        const QwtInterval &interval, double stepSize ) const;

    void buildMinorTicks(
        const QList<double>& majorTicks,
        int maxMinMark, double step,
        QList<double> &, QList<double> & ) const;
};

/*!
  \brief A scale engine for logarithmic (base 10) scales

  The step size is measured in *decades*
  and the major step size will be adjusted to fit the pattern
  \f$\left\{ 1,2,3,5\right\} \cdot 10^{n}\f$, where n is a natural number
  including zero.

  \warning the step size is measured in *decades*.
*/

class QwtLog10ScaleEngine: public QwtScaleEngine
{
public:
    virtual QwtScaleDiv divideScale( double x1, double x2,
        int numMajorSteps, int numMinorSteps,
        double stepSize = 0.0 ) const;

    virtual QwtScaleTransformation *transformation() const;

protected:
    QwtInterval log10( const QwtInterval& ) const;
    QwtInterval pow10( const QwtInterval& ) const;

    QwtInterval align( const QwtInterval&, double stepSize ) const;

    void buildTicks(
        const QwtInterval &, double stepSize, int maxMinSteps,
        QList<double> ticks[QwtScaleDiv::NTickTypes] ) const;

    QList<double> buildMajorTicks(
        const QwtInterval &interval, double stepSize ) const;

    QList<double> buildMinorTicks(
        const QList<double>& majorTicks,
        int maxMinMark, double step ) const;
};
