#pragma once

#include "qwt_text.h"
#include <qframe.h>

class QString;
class QPaintEvent;
class QPainter;

/*!
   \brief A Widget which displays a QwtText
*/

class QwtTextLabel : public QFrame
{
    Q_OBJECT

public:
    explicit QwtTextLabel( QWidget *parent = NULL );
    explicit QwtTextLabel( const QwtText &, QWidget *parent = NULL );
    virtual ~QwtTextLabel();

public Q_SLOTS:
    void setText( const QString & );
    virtual void setText( const QwtText & );

    void clear();

public:
    const QwtText &text() const;

    int indent() const;
    void setIndent( int );

    int margin() const;
    void setMargin( int );

    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;
    virtual int heightForWidth( int ) const;

    QRect textRect() const;

protected:
    virtual void paintEvent( QPaintEvent *e );
    virtual void drawContents( QPainter * );
    virtual void drawText( QPainter *, const QRect & );

private:
    void init();
    int defaultIndent() const;

    class PrivateData;
    PrivateData *d_data;
};
