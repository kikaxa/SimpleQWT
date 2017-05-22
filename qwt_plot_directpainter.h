#pragma once

#include <qobject.h>

class QRegion;
class QwtPlotAbstractSeriesItem;

/*!
    \brief Painter object trying to paint incrementally

    Often applications want to display samples while they are
    collected. When there are too many samples complete replots
    will be expensive to be processed in a collection cycle.

    QwtPlotDirectPainter offers an API to paint
    subsets ( f.e all additions points ) without erasing/repainting
    the plot canvas.

    \warning Incremental painting will only help when no replot is triggered
             by another operation ( like changing scales ) and nothing needs
             to be erased.
*/
class QwtPlotDirectPainter: public QObject
{
public:
    QwtPlotDirectPainter( QObject *parent = NULL );
    virtual ~QwtPlotDirectPainter();

    void drawSeries( QwtPlotAbstractSeriesItem *, int from, int to );
    void reset();

    virtual bool eventFilter( QObject *, QEvent * );

private:
    class PrivateData;
    PrivateData *d_data;
};
