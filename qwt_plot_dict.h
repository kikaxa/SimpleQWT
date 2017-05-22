#pragma once

#include "qwt_plot_item.h"
#include <qlist.h>

/// \var typedef QList< QwtPlotItem *> QwtPlotItemList
/// \brief See QT 4.x assistant documentation for QList
typedef QList<QwtPlotItem *> QwtPlotItemList;
typedef QList<QwtPlotItem *>::ConstIterator QwtPlotItemIterator;

/*!
  \brief A dictionary for plot items

  QwtPlotDict organizes plot items in increasing z-order.
  If autoDelete() is enabled, all attached items will be deleted
  in the destructor of the dictionary.

  \sa QwtPlotItem::attach(), QwtPlotItem::detach(), QwtPlotItem::z()
*/
class QwtPlotDict
{
public:
    explicit QwtPlotDict();
    virtual ~QwtPlotDict();

    void setAutoDelete( bool );
    bool autoDelete() const;

    const QwtPlotItemList& itemList() const;

    void detachItems( bool autoDelete = true );

private:
    friend class QwtPlotItem;

    void attachItem( QwtPlotItem *, bool );

    class PrivateData;
    PrivateData *d_data;
};
