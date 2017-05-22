#pragma once

#include "qwt_interval.h"
#include <qcolor.h>
#include <qvector.h>

/*!
  \brief QwtColorMap is used to map values into colors.

  For displaying 3D data on a 2D plane the 3rd dimension is often
  displayed using colors, like f.e in a spectrogram.

  \sa QwtPlotSpectrogram, QwtScaleWidget
*/

class QwtColorMap
{
public:
    virtual ~QwtColorMap() { }

    /*!
       Map a value of a given interval into a rgb value.
       \param interval Range for the values
       \param value Value
       \return rgb value, corresponding to value
    */
    virtual QRgb rgb( const QwtInterval &interval,
        double value ) const = 0;
};

/*!
  \brief QwtLinearColorMap builds a color map from color stops.

  A color stop is a color at a specific position. The valid
  range for the positions is [0.0, 1.0]. When mapping a value
  into a color it is translated into this interval according to mode().
*/
class QwtLinearColorMap: public QwtColorMap
{
public:
    /*!
       Mode of color map
       \sa setMode(), mode()
    */
    enum Mode
    {
        //! Return the color from the next lower color stop
        FixedColors,

        //! Interpolating the colors of the adjacent stops.
        ScaledColors
    };

    QwtLinearColorMap( );
    QwtLinearColorMap( const QColor &from, const QColor &to );

    virtual ~QwtLinearColorMap();

    void setMode( Mode );
    Mode mode() const;

    void setColorInterval( const QColor &color1, const QColor &color2 );
    void addColorStop( double value, const QColor& );
    QVector<double> colorStops() const;

    QColor color1() const;
    QColor color2() const;

    virtual QRgb rgb( const QwtInterval &, double value ) const;

    class ColorStops;

private:
    // Disabled copy constructor and operator=
    QwtLinearColorMap( const QwtLinearColorMap & );
    QwtLinearColorMap &operator=( const QwtLinearColorMap & );

    class PrivateData;
    PrivateData *d_data;
};

/*!
  \brief QwtAlphaColorMap variies the alpha value of a color
*/
class QwtAlphaColorMap: public QwtColorMap
{
public:
    QwtAlphaColorMap( const QColor & = QColor( Qt::gray ) );
    virtual ~QwtAlphaColorMap();

    void setColor( const QColor & );
    QColor color() const;

    virtual QRgb rgb( const QwtInterval &, double value ) const;

private:
    QwtAlphaColorMap( const QwtAlphaColorMap & );
    QwtAlphaColorMap &operator=( const QwtAlphaColorMap & );

    class PrivateData;
    PrivateData *d_data;
};
