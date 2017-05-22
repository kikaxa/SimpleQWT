#pragma once

class QwtScaleEngine;
class QwtScaleDraw;
class QwtScaleDiv;
class QwtScaleMap;
class QwtInterval;

/*!
  \brief An abstract base class for classes containing a scale

  QwtAbstractScale is used to provide classes with a QwtScaleDraw,
  and a QwtScaleDiv. The QwtScaleDiv might be set explicitely
  or calculated by a QwtScaleEngine.
*/

class QwtAbstractScale
{
public:
    QwtAbstractScale();
    ~QwtAbstractScale();

    void setScale( double vmin, double vmax, double step = 0.0 );
    void setScale( const QwtInterval &, double step = 0.0 );
    void setScale( const QwtScaleDiv & );

    void setScaleEngine( QwtScaleEngine * );
    void setAbstractScaleDraw( QwtScaleDraw * );

    void updateScaleDraw();
    void rescale( double vmin, double vmax, double step = 0.0 );
    virtual void scaleChange();

    QwtScaleEngine *scaleEngine;
    QwtScaleDraw *scaleDraw;
    double stepSize;
};
