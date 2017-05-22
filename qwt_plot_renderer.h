#pragma once

class QWidget;
class QwtPlot;
class QwtScaleMap;
class QRectF;
class QPainter;

/*!
    \brief Renderer for exporting a plot to a document, a printer
           or anything else, that is supported by QPainter/QPaintDevice
*/
class QwtPlotRenderer
{
public:
    virtual void render( QwtPlot *,
        QPainter *, const QRectF &rect ) const;

    virtual void renderLegendItem( const QwtPlot *, 
        QPainter *, const QWidget *, const QRectF & ) const;

    virtual void renderTitle( const QwtPlot *,
        QPainter *, const QRectF & ) const;

    virtual void renderScale( const QwtPlot *, QPainter *,
        int axisId, int baseDist, const QRectF & ) const;

    virtual void renderCanvas( const QwtPlot *,
        QPainter *, const QRectF &canvasRect,
        const QwtScaleMap* maps ) const;

    virtual void renderLegend( 
        const QwtPlot *, QPainter *, const QRectF & ) const;

protected:
    void buildCanvasMaps( const QwtPlot *,
        const QRectF &, QwtScaleMap maps[] ) const;
};
