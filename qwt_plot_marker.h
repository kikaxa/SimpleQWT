#pragma once

#include <qpen.h>
#include <qfont.h>
#include <qstring.h>
#include <qbrush.h>

#include "qwt_plot_item.h"

class QRectF;
class QwtText;
class QwtSymbol;

/*!
  \brief A class for drawing markers

  A marker can be a horizontal line, a vertical line,
  a symbol, a label or any combination of them, which can
  be drawn around a center point inside a bounding rectangle.

  The QwtPlotMarker::setSymbol() member assigns a symbol to the marker.
  The symbol is drawn at the specified point.

  With setLabel(), a label can be assigned to the marker.
  The setLabelAlignment() member specifies where the label is
  drawn. All the Align*-constants in Qt::AlignmentFlags (see Qt documentation)
  are valid. The interpretation of the alignment depends on the marker's
  line style. The alignment refers to the center point of
  the marker, which means, for example, that the label would be printed
  left above the center point if the alignment was set to 
  Qt::AlignLeft | Qt::AlignTop.
*/

class QwtPlotMarker: public QwtPlotItem
{
public:

    /*!
        Line styles.
        \sa setLineStyle(), lineStyle()
    */
    enum LineStyle
    {
        //! No line
        NoLine,

        //! A horizontal line
        HLine,

        //! A vertical line
        VLine,

        //! A crosshair
        Cross
    };

    explicit QwtPlotMarker();
    virtual ~QwtPlotMarker();

    double xValue() const;
    double yValue() const;
    QPointF value() const;

    void setXValue( double );
    void setYValue( double );
    void setValue( double, double );
    void setValue( const QPointF & );

    void setLineStyle( LineStyle st );
    LineStyle lineStyle() const;

    void setLinePen( const QPen &p );
    const QPen &linePen() const;

    void setSymbol( const QwtSymbol * );
    const QwtSymbol *symbol() const;

    void setLabel( const QwtText& );
    QwtText label() const;

    void setLabelAlignment( Qt::Alignment );
    Qt::Alignment labelAlignment() const;

    void setLabelOrientation( Qt::Orientation );
    Qt::Orientation labelOrientation() const;

    void setSpacing( int );
    int spacing() const;

    virtual void draw( QPainter *p,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF & ) const;

    virtual QRectF boundingRect() const;

    virtual void updateLegend( QwtLegend * ) const;
    virtual void drawLegendIdentifier( QPainter *, const QRectF & ) const;

protected:
    virtual void drawLines( QPainter *, 
        const QRectF &, const QPointF & ) const;

    virtual void drawLabel( QPainter *, 
        const QRectF &, const QPointF & ) const;

private:

    class PrivateData;
    PrivateData *d_data;
};
