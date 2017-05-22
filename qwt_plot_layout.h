#pragma once

#include "qwt_plot.h"

/*!
  \brief Layout engine for QwtPlot.

  It is used by the QwtPlot widget to organize its internal widgets
  or by QwtPlot::print() to render its content to a QPaintDevice like
  a QPrinter, QPixmap/QImage or QSvgRenderer.
*/

class QwtPlotLayout
{
public:
    explicit QwtPlotLayout();
    virtual ~QwtPlotLayout();

    void setSpacing( int );
    int spacing() const;

    void setLegendPosition( QwtPlot::LegendPosition pos, double ratio );
    void setLegendPosition( QwtPlot::LegendPosition pos );
    QwtPlot::LegendPosition legendPosition() const;

    void setLegendRatio( double ratio );
    double legendRatio() const;

    virtual QSize minimumSizeHint( const QwtPlot * ) const;

    virtual void activate( const QwtPlot *,
        const QRectF &rect);

    virtual void invalidate();

    const QRectF &titleRect() const;
    const QRectF &legendRect() const;
    const QRectF &scaleRect( int axis ) const;
    const QRectF &canvasRect() const;

    class LayoutData;

protected:

    QRectF layoutLegend( const QRectF & ) const;
    QRectF alignLegend( const QRectF &canvasRect,
        const QRectF &legendRect ) const;

    void expandLineBreaks( const QRectF &rect,
        int &dimTitle, int dimAxes[QwtPlot::axisCnt] ) const;

private:
    class PrivateData;

    PrivateData *d_data;
};
