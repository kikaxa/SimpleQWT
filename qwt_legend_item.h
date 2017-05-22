#pragma once

#include "qwt_legend.h"
#include "qwt_text.h"
#include "qwt_text_label.h"
#include <qpixmap.h>

/*!
  \brief A widget representing something on a QwtLegend().
*/
class QwtLegendItem: public QwtTextLabel
{
    Q_OBJECT
public:
    explicit QwtLegendItem( QWidget *parent = 0 );
    virtual ~QwtLegendItem();

    void setSpacing( int spacing );
    int spacing() const;

    virtual void setText( const QwtText & );

    void setIdentifier( const QPixmap & );
    QPixmap identifier() const;

    void setIdentifierSize( const QSize & );
    QSize identifierSize() const;

    virtual QSize sizeHint() const;

protected:
    virtual void paintEvent( QPaintEvent * );

private:
    class PrivateData;
    PrivateData *d_data;
};
