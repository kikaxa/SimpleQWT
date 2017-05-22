#pragma once

#include "qwt_raster_data.h"
#include "qwt_plot_rasteritem.h"
#include <qlist.h>

class QwtColorMap;

/*!
  \brief A plot item, which displays a spectrogram

  A spectrogram displays threedimenional data, where the 3rd dimension
  ( the intensity ) is displayed using colors. The colors are calculated
  from the values using a color map.

  In ContourMode contour lines are painted for the contour levels.

  \image html spectrogram3.png

  \sa QwtRasterData, QwtColorMap
*/

class QwtPlotSpectrogram: public QwtPlotRasterItem
{
public:
    /*!
      The display mode controls how the raster data will be represented.
      \sa setDisplayMode(), testDisplayMode()
    */

    enum DisplayMode
    {
        //! The values are mapped to colors using a color map.
        ImageMode = 0x01,

        //! The data is displayed using contour lines
        ContourMode = 0x02
    };

    //! Display modes
    typedef QFlags<DisplayMode> DisplayModes;

    explicit QwtPlotSpectrogram( const QString &title = QString::null );
    virtual ~QwtPlotSpectrogram();

    void setRenderThreadCount( uint numThreads );
    uint renderThreadCount() const;

    void setDisplayMode( DisplayMode, bool on = true );
    bool testDisplayMode( DisplayMode ) const;

    void setData( QwtRasterData *data );
    const QwtRasterData *data() const;
    QwtRasterData *data();

    void setColorMap( QwtColorMap * );
    const QwtColorMap *colorMap() const;

    virtual QwtInterval interval(Qt::Axis) const;
    virtual QRectF pixelHint( const QRectF & ) const;

    void setDefaultContourPen( const QPen & );
    QPen defaultContourPen() const;

    virtual QPen contourPen( double level ) const;

    void setConrecFlag( QwtRasterData::ConrecFlag, bool on );
    bool testConrecFlag( QwtRasterData::ConrecFlag ) const;

    void setContourLevels( const QList<double> & );
    QList<double> contourLevels() const;

    virtual void draw( QPainter *p,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF &rect ) const;

protected:
    virtual QImage renderImage(
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF &area, const QSize &imageSize ) const;

    virtual QSize contourRasterSize(
        const QRectF &, const QRect & ) const;

    virtual QwtRasterData::ContourLines renderContourLines(
        const QRectF &rect, const QSize &raster ) const;

    virtual void drawContourLines( QPainter *p,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QwtRasterData::ContourLines& lines ) const;

    void renderTile( const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRect &imageRect, QImage *image ) const;

private:
    class PrivateData;
    PrivateData *d_data;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( QwtPlotSpectrogram::DisplayModes )
