#pragma once

#include <qpoint.h>
#include <qrect.h>

class QPainter;
class QWidget;
class QRectF;
class QImage;
class QPixmap;
class QwtScaleMap;
class QwtColorMap;
class QwtInterval;

class QTextDocument;

/*!
  \brief A collection of QPainter workarounds
*/
class QwtPainter
{
public:
    static void unscaleFont( QPainter *painter );

    static void drawImage( QPainter *, const QRectF &, const QImage & );

    static void drawFocusRect( QPainter *, QWidget *, const QRect & );

    static void drawColorBar( QPainter *painter,
        const QwtColorMap &, const QwtInterval &,
        const QwtScaleMap &, Qt::Orientation, const QRectF & );
};
