/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_legend_item.h"
#include "qwt_symbol.h"
#include <qpainter.h>
#include <qdrawutil.h>
#include <qstyle.h>
#include <qpen.h>
#include <qevent.h>
#include <qstyleoption.h>
#include <qapplication.h>

static const int Margin = 2;

class QwtLegendItem::PrivateData
{
public:
    PrivateData():
        identifierSize( 8, 8 ),
        spacing( Margin )
    {
    }

    QSize identifierSize;
    QPixmap identifier;

    int spacing;
};

/*!
  \param parent Parent widget
*/
QwtLegendItem::QwtLegendItem( QWidget *parent ):
    QwtTextLabel( parent )
{
    d_data = new PrivateData;
    setMargin( Margin );
    setIndent( Margin + d_data->identifierSize.width() + 2 * d_data->spacing );
    setFocusPolicy( Qt::NoFocus );
}

//! Destructor
QwtLegendItem::~QwtLegendItem()
{
    delete d_data;
    d_data = NULL;
}

/*!
   Set the text to the legend item

   \param text Text label
    \sa QwtTextLabel::text()
*/
void QwtLegendItem::setText( const QwtText &text )
{
    const int flags = Qt::AlignLeft | Qt::AlignVCenter
        | Qt::TextExpandTabs | Qt::TextWordWrap;

    QwtText txt = text;
    txt.setRenderFlags( flags );

    QwtTextLabel::setText( txt );
}


/*!
  Assign the identifier
  The identifier needs to be created according to the identifierWidth()

  \param identifier Pixmap representing a plot item

  \sa identifier(), identifierWidth()
*/
void QwtLegendItem::setIdentifier( const QPixmap &identifier )
{
    d_data->identifier = identifier;
    update();
}

/*!
  \return pixmap representing a plot item
  \sa setIdentifier()
*/
QPixmap QwtLegendItem::identifier() const
{
    return d_data->identifier;
}

/*!
  Set the size for the identifier
  Default is 8x8 pixels

  \param size New size

  \sa identifierSize()
*/
void QwtLegendItem::setIdentifierSize( const QSize &size )
{
    QSize sz = size.expandedTo( QSize( 0, 0 ) );
    if ( sz != d_data->identifierSize )
    {
        d_data->identifierSize = sz;
        setIndent( margin() + d_data->identifierSize.width()
            + 2 * d_data->spacing );
        updateGeometry();
    }
}
/*!
   Return the width of the identifier

   \sa setIdentifierSize()
*/
QSize QwtLegendItem::identifierSize() const
{
    return d_data->identifierSize;
}

/*!
   Change the spacing
   \param spacing Spacing
   \sa spacing(), identifierWidth(), QwtTextLabel::margin()
*/
void QwtLegendItem::setSpacing( int spacing )
{
    spacing = qMax( spacing, 0 );
    if ( spacing != d_data->spacing )
    {
        d_data->spacing = spacing;
        setIndent( margin() + d_data->identifierSize.width()
            + 2 * d_data->spacing );
    }
}

/*!
   Return the spacing
   \sa setSpacing(), identifierWidth(), QwtTextLabel::margin()
*/
int QwtLegendItem::spacing() const
{
    return d_data->spacing;
}

//! Return a size hint
QSize QwtLegendItem::sizeHint() const
{
    QSize sz = QwtTextLabel::sizeHint();
    sz.setHeight( qMax( sz.height(), d_data->identifier.height() + 4 ) );
    return sz;
}

//! Paint event
void QwtLegendItem::paintEvent( QPaintEvent *e )
{
    const QRect cr = contentsRect();

    QPainter painter( this );
    painter.setClipRegion( e->region() );
    painter.save();
    painter.setClipRect( cr );

    drawContents( &painter );

    if ( !d_data->identifier.isNull() )
    {
        QRect identRect = cr;
        identRect.setX( identRect.x() + margin() );

        identRect.setSize( d_data->identifier.size() );
        identRect.moveCenter( QPoint( identRect.center().x(), cr.center().y() ) );

        painter.drawPixmap( identRect, d_data->identifier );
    }

    painter.restore();
}
