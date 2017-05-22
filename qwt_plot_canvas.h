#pragma once

#include <qframe.h>

class QwtPlot;

/*!
  \brief Canvas of a QwtPlot.
  \sa QwtPlot
*/
class QwtPlotCanvas : public QFrame
{
    Q_OBJECT

public:
    QwtPlotCanvas( QwtPlot * );

protected:
    virtual void paintEvent( QPaintEvent * );
};
