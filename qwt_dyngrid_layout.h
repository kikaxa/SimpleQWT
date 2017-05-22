#pragma once

#include <qlayout.h>
#include <qsize.h>
#include <qlist.h>

/*!
  \brief The QwtDynGridLayout class lays out widgets in a grid,
         adjusting the number of columns and rows to the current size.

  QwtDynGridLayout takes the space it gets, divides it up into rows and
  columns, and puts each of the widgets it manages into the correct cell(s).
  It lays out as many number of columns as possible (limited by maxCols()).
*/

class QwtDynGridLayout : public QLayout
{
    Q_OBJECT
public:
    explicit QwtDynGridLayout( QWidget *, int margin = 0, int space = -1 );
    explicit QwtDynGridLayout( int space = -1 );

    virtual ~QwtDynGridLayout();

    virtual void invalidate();

    void setMaxCols( uint maxCols );
    uint maxCols() const;

    uint numRows () const;
    uint numCols () const;

    virtual void addItem( QLayoutItem * );

    virtual QLayoutItem *itemAt( int index ) const;
    virtual QLayoutItem *takeAt( int index );
    virtual int count() const;

    void setExpandingDirections( Qt::Orientations );
    virtual Qt::Orientations expandingDirections() const;
    QList<QRect> layoutItems( const QRect &, uint numCols ) const;

    virtual int maxItemWidth() const;

    virtual void setGeometry( const QRect &rect );

    virtual bool hasHeightForWidth() const;
    virtual int heightForWidth( int ) const;

    virtual QSize sizeHint() const;

    virtual bool isEmpty() const;
    uint itemCount() const;

    virtual uint columnsForWidth( int width ) const;

protected:

    void layoutGrid( uint numCols,
        QVector<int>& rowHeight, QVector<int>& colWidth ) const;
    void stretchGrid( const QRect &rect, uint numCols,
        QVector<int>& rowHeight, QVector<int>& colWidth ) const;

private:
    void init();
    int maxRowWidth( int numCols ) const;

    class PrivateData;
    PrivateData *d_data;
};
