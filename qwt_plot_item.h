#pragma once

#include "qwt_legend_itemmanager.h"
#include "qwt_text.h"
#include <qrect.h>

class QString;
class QPainter;
class QWidget;
class QwtPlot;
class QwtLegend;
class QwtScaleMap;
class QwtScaleDiv;

/*!
  \brief Base class for items on the plot canvas

  A plot item is "something", that can be painted on the plot canvas,
  or only affects the scales of the plot widget. They can be categorized as:

  - Representator\n
    A "Representator" is an item that represents some sort of data
    on the plot canvas. The different representator classes are organized
    according to the characteristics of the data:
    - QwtPlotMarker
      Represents a point or a horizontal/vertical coordinate
    - QwtPlotCurve
      Represents a series of points
    - QwtPlotSpectrogram ( QwtPlotRasterItem )
      Represents raster data
    - ...

  - Decorators\n
    A "Decorator" is an item, that displays additional information, that
    is not related to any data:
    - QwtPlotGrid
    - QwtPlotScaleItem
    - QwtPlotSvgItem
    - ...

  Depending on the QwtPlotItem::ItemAttribute flags, an item is included
  into autoscaling or has an entry on the legnd.

  Before misusing the existing item classes it might be better to
  implement a new type of plot item
  ( don't implement a watermark as spectrogram ).
  Deriving a new type of QwtPlotItem primarily means to implement
  the YourPlotItem::draw() method.

  \sa The cpuplot example shows the implementation of additional plot items.
*/

class QwtPlotItem: public QwtLegendItemManager
{
public:
    //! Render hints
    enum RenderHint
    {
        //! Enable antialiasing
        RenderAntialiased = 1
    };

    //! Render hints
    typedef QFlags<RenderHint> RenderHints;

    explicit QwtPlotItem( const QwtText &title = QwtText() );
    virtual ~QwtPlotItem();

    void attach( QwtPlot *plot );
    void detach();

    QwtPlot *plot() const;

    void setTitle( const QString &title );
    void setTitle( const QwtText &title );
    const QwtText &title() const;

    void setRenderHint( RenderHint, bool on = true );
    bool testRenderHint( RenderHint ) const;

    double z() const;
    void setZ( double z );

    void show();
    void hide();
    virtual void setVisible( bool );
    bool isVisible () const;

    virtual void itemChanged();

    /*!
      \brief Draw the item

      \param painter Painter
      \param xMap Maps x-values into pixel coordinates.
      \param yMap Maps y-values into pixel coordinates.
      \param canvasRect Contents rect of the canvas in painter coordinates
    */
    virtual void draw( QPainter *painter,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF &canvasRect ) const = 0;

    virtual QRectF boundingRect() const;

    virtual void updateLegend( QwtLegend * ) const;

    virtual QWidget *legendItem() const;

    QRectF scaleRect( const QwtScaleMap &, const QwtScaleMap & ) const;
    QRectF paintRect( const QwtScaleMap &, const QwtScaleMap & ) const;

private:
    // Disabled copy constructor and operator=
    QwtPlotItem( const QwtPlotItem & );
    QwtPlotItem &operator=( const QwtPlotItem & );

    class PrivateData;
    PrivateData *d_data;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( QwtPlotItem::RenderHints )
