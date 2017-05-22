/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_plot_canvas.h"
#include "qwt_plot.h"
#include <qpainter.h>
#include <qstyle.h>
#include <qstyleoption.h>
#include <qevent.h>


//! Sets a cross cursor

QwtPlotCanvas::QwtPlotCanvas( QwtPlot *plot ):
    QFrame( plot )
{
#ifndef QT_NO_CURSOR
    setCursor( Qt::CrossCursor );
#endif

    setAttribute( Qt::WA_OpaquePaintEvent, true );
}


static inline void qwtDrawStyledBackground(
    QWidget *w, QPainter *painter )
{
    QStyleOption opt;
    opt.initFrom(w);
    w->style()->drawPrimitive( QStyle::PE_Widget, &opt, painter, w);
}

/*!
  Paint event
  \param event Paint event
*/
void QwtPlotCanvas::paintEvent( QPaintEvent *event )
{
    QPainter painter( this );
    painter.setClipRegion( event->region() );

    painter.save();

    painter.setPen( Qt::NoPen );
    painter.setBrush( palette().brush( backgroundRole() ) );
    painter.drawRect( contentsRect() );

    if ( testAttribute( Qt::WA_StyledBackground ) )
        qwtDrawStyledBackground( this, &painter );

    painter.restore();

    painter.save();

    painter.setClipRect( contentsRect(), Qt::IntersectClip );

    QwtPlot *plot = qobject_cast<QwtPlot *>( parentWidget() );
    plot->drawCanvas( &painter );

    painter.restore();

    if ( !testAttribute(Qt::WA_StyledBackground ) && frameWidth() > 0 )
        drawFrame( &painter );
}
