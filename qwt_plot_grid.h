#pragma once

#include "qwt_plot_item.h"
#include "qwt_scale_div.h"

class QPainter;
class QPen;
class QwtScaleMap;
class QwtScaleDiv;

/*!
  \brief A class which draws a coordinate grid

  The QwtPlotGrid class can be used to draw a coordinate grid.
  A coordinate grid consists of major and minor vertical
  and horizontal gridlines. The locations of the gridlines
  are determined by the X and Y scale divisions which can
  be assigned with setXDiv() and setYDiv().
  The draw() member draws the grid within a bounding
  rectangle.
*/

class QwtPlotGrid: public QwtPlotItem
{
public:
    explicit QwtPlotGrid();
    virtual ~QwtPlotGrid();

    void enableX( bool tf );
    bool xEnabled() const;

    void enableY( bool tf );
    bool yEnabled() const;

    void enableXMin( bool tf );
    bool xMinEnabled() const;

    void enableYMin( bool tf );
    bool yMinEnabled() const;

    void setPen( const QPen &p );

    void setMajPen( const QPen &p );
    const QPen& majPen() const;

    void setMinPen( const QPen &p );
    const QPen& minPen() const;

    virtual void draw( QPainter *p,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF &rect ) const;

private:
    void drawLines( QPainter *painter, const QRectF &,
        Qt::Orientation orientation, const QwtScaleMap &,
        const QList<double> & ) const;

    class PrivateData;
    PrivateData *d_data;
};
