#pragma once

#include "qwt_text.h"
#include "qwt_scale_draw.h"
#include <qwidget.h>
#include <qfont.h>
#include <qcolor.h>
#include <qstring.h>

class QPainter;
class QwtScaleTransformation;
class QwtScaleDiv;
class QwtColorMap;

/*!
  \brief A Widget which contains a scale

  This Widget can be used to decorate composite widgets with
  a scale.
*/

class QwtScaleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit QwtScaleWidget( QWidget *parent = NULL );
    explicit QwtScaleWidget( QwtScaleDraw::Alignment, QWidget *parent = NULL );
    virtual ~QwtScaleWidget();

Q_SIGNALS:
    //! Signal emitted, whenever the scale divison changes
    void scaleDivChanged();

public:
    void setTitle( const QString &title );
    void setTitle( const QwtText &title );
    QwtText title() const;

    void setBorderDist( int start, int end );
    int startBorderDist() const;
    int endBorderDist() const;

    void setMargin( int );
    int margin() const;

    void setSpacing( int td );
    int spacing() const;

    void setScaleDiv( QwtScaleTransformation *, const QwtScaleDiv &sd );

    void setScaleDraw( QwtScaleDraw * );
    const QwtScaleDraw *scaleDraw() const;
    QwtScaleDraw *scaleDraw();

    void setColorBarEnabled( bool );
    bool isColorBarEnabled() const;

    void setColorBarWidth( int );
    int colorBarWidth() const;

    void setColorMap( const QwtInterval &, QwtColorMap * );

    QwtInterval colorBarInterval() const;
    const QwtColorMap *colorMap() const;

    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;

    int titleHeightForWidth( int width ) const;
    int dimForLength( int length, const QFont &scaleFont ) const;

    void drawColorBar( QPainter *painter, const QRectF & ) const;
    void drawTitle( QPainter *painter, QwtScaleDraw::Alignment,
        const QRectF &rect ) const;

    void setAlignment( QwtScaleDraw::Alignment );
    QwtScaleDraw::Alignment alignment() const;

    QRectF colorBarRect( const QRectF& ) const;

protected:
    virtual void paintEvent( QPaintEvent * );
    virtual void resizeEvent( QResizeEvent * );

    void draw( QPainter *p ) const;

    void scaleChange();
    void layoutScale( bool update = true );

private:
    void initScale( QwtScaleDraw::Alignment );

    class PrivateData;
    PrivateData *d_data;
};
