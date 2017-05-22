#pragma once

#include <qframe.h>
#include <qlist.h>

class QScrollBar;
class QwtLegendItemManager;

/*!
  \brief The legend widget

  The QwtLegend widget is a tabular arrangement of legend items. Legend
  items might be any type of widget, but in general they will be
  a QwtLegendItem.

  \sa QwtLegendItem, QwtLegendItemManager QwtPlot
*/

class QwtLegend : public QFrame
{
    Q_OBJECT

public:
    explicit QwtLegend( QWidget *parent = NULL );
    virtual ~QwtLegend();

    QWidget *contentsWidget();
    const QWidget *contentsWidget() const;

    void insert( const QwtLegendItemManager *, QWidget * );
    void remove( const QwtLegendItemManager * );

    QWidget *find( const QwtLegendItemManager * ) const;
    QwtLegendItemManager *find( const QWidget * ) const;

    virtual QList<QWidget *> legendItems() const;

    void clear();

    bool isEmpty() const;
    uint itemCount() const;

    virtual bool eventFilter( QObject *, QEvent * );

    virtual QSize sizeHint() const;
    virtual int heightForWidth( int w ) const;

    QScrollBar *horizontalScrollBar() const;
    QScrollBar *verticalScrollBar() const;

protected:
    virtual void layoutContents();

private:
    class PrivateData;
    PrivateData *d_data;
};
