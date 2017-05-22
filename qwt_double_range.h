#pragma once

/*!
  \brief A class which controls a value within an interval

  This class is useful as a base class or a member for sliders.
  It represents an interval of type double within which a value can
  be moved. The value can be either an arbitrary point inside
  the interval (see QwtDoubleRange::setValue), or it can be fitted
  into a step raster (see QwtDoubleRange::fitValue and
  QwtDoubleRange::incValue).
*/

class QwtDoubleRange
{
public:
    QwtDoubleRange();

    void setRange( double vmin, double vmax, 
        double vstep = 0.0 );

    virtual void setValue( double );
    void setStep( double );

    double minValue;
    double maxValue;
    double step;
    double value;

protected:
    virtual void valueChange() { }
    virtual void rangeChange() { }

    void setNewValue( double value, bool align = false );

};
