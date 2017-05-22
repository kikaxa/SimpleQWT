/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_plot_renderer.h"
#include "qwt_plot.h"
#include "qwt_plot_canvas.h"
#include "qwt_plot_layout.h"
#include "qwt_legend.h"
#include "qwt_legend_item.h"
#include "qwt_dyngrid_layout.h"
#include "qwt_scale_widget.h"
#include "qwt_scale_engine.h"
#include "qwt_text.h"
#include "qwt_text_label.h"
#include <qpainter.h>
#include <qtransform.h>
#include <qstyle.h>
#include <qstyleoption.h>

static void qwtRenderBackground( QPainter *painter,
    const QRectF &rect, const QWidget *widget )
{
    if ( widget->testAttribute( Qt::WA_StyledBackground ) )
    {
        QStyleOption opt;
        opt.initFrom( widget );
        opt.rect = rect.toAlignedRect();

        widget->style()->drawPrimitive(
            QStyle::PE_Widget, &opt, painter, widget);
    }
    else
    {
        const QBrush brush = 
            widget->palette().brush( widget->backgroundRole() );

        painter->fillRect( rect, brush );
    }
}

/*!
  Paint the contents of a QwtPlot instance into a given rectangle.

  \param plot Plot to be rendered
  \param painter Painter
  \param plotRect Bounding rectangle

  \sa renderDocument(), renderTo(), QwtPainter::setRoundingAlignment()
*/
void QwtPlotRenderer::render( QwtPlot *plot,
    QPainter *painter, const QRectF &plotRect ) const
{
    int axisId;

    if ( painter == 0 || !painter->isActive() ||
            !plotRect.isValid() || plot->size().isNull() )
        return;

    //qwtRenderBackground( painter, plotRect, plot );

    /*
      The layout engine uses the same methods as they are used
      by the Qt layout system. Therefore we need to calculate the
      layout in screen coordinates and paint with a scaled painter.
     */
    QTransform transform;
    transform.scale(
        double( painter->device()->logicalDpiX() ) / plot->logicalDpiX(),
        double( painter->device()->logicalDpiY() ) / plot->logicalDpiY() );

    painter->save();

    // Calculate the layout for the print.

    const QRectF layoutRect = transform.inverted().mapRect( plotRect );
    plot->plotLayout()->activate( plot, layoutRect );

    painter->setWorldTransform( transform, true );

    // canvas

    QwtScaleMap maps[QwtPlot::axisCnt];
    buildCanvasMaps( plot, plot->plotLayout()->canvasRect(), maps );
    renderCanvas( plot, painter, plot->plotLayout()->canvasRect(), maps );

    if ( !plot->titleLabel()->text().isEmpty() )
    {
        renderTitle( plot, painter, plot->plotLayout()->titleRect() );
    }

    if ( plot->legend() && !plot->legend()->isEmpty() )
    {
        renderLegend( plot, painter, plot->plotLayout()->legendRect() );
    }

    for ( axisId = 0; axisId < QwtPlot::axisCnt; axisId++ )
    {
        QwtScaleWidget *scaleWidget = plot->axisWidget( axisId );
        if ( scaleWidget )
        {
            renderScale( plot, painter, axisId,
                scaleWidget->margin(), plot->plotLayout()->scaleRect( axisId ) );
        }
    }


    plot->plotLayout()->invalidate();

    painter->restore();
}

/*!
  Render the title into a given rectangle.

  \param plot Plot widget
  \param painter Painter
  \param rect Bounding rectangle
*/
void QwtPlotRenderer::renderTitle( const QwtPlot *plot,
    QPainter *painter, const QRectF &rect ) const
{
    painter->setFont( plot->titleLabel()->font() );

    const QColor color = plot->titleLabel()->palette().color(
            QPalette::Active, QPalette::Text );

    painter->setPen( color );
    plot->titleLabel()->text().draw( painter, rect );
}

/*!
  Render the legend into a given rectangle.

  \param plot Plot widget
  \param painter Painter
  \param rect Bounding rectangle
*/
void QwtPlotRenderer::renderLegend( const QwtPlot *plot,
    QPainter *painter, const QRectF &rect ) const
{
    if ( !plot->legend() || plot->legend()->isEmpty() )
        return;

    if ( plot->legend()->autoFillBackground() ||
        plot->legend()->testAttribute( Qt::WA_StyledBackground ) )
    {
        //qwtRenderBackground( painter, rect, plot->legend() );
    }

    QLayout *l = plot->legend()->contentsWidget()->layout();
    if ( l == 0 || !l->inherits( "QwtDynGridLayout" ) )
        return;

    QwtDynGridLayout *legendLayout = ( QwtDynGridLayout * )l;

    uint numCols = legendLayout->columnsForWidth( rect.width() );
    QList<QRect> itemRects =
        legendLayout->layoutItems( rect.toRect(), numCols );

    int index = 0;

    for ( int i = 0; i < legendLayout->count(); i++ )
    {
        QLayoutItem *item = legendLayout->itemAt( i );
        QWidget *w = item->widget();
        if ( w )
        {
            painter->save();

            painter->setClipRect( itemRects[index] );
            renderLegendItem( plot, painter, w, itemRects[index] );

            index++;
            painter->restore();
        }
    }
}

/*!
  Render the legend item into a given rectangle.

  \param plot Plot widget
  \param painter Painter
  \param widget Widget representing a legend item
  \param rect Bounding rectangle

  \note When widget is not derived from QwtLegendItem renderLegendItem
        does nothing and needs to be overloaded
*/
void QwtPlotRenderer::renderLegendItem( const QwtPlot *plot,
    QPainter *painter, const QWidget *widget, const QRectF &rect ) const
{
    if ( widget->autoFillBackground() ||
        widget->testAttribute( Qt::WA_StyledBackground ) )
    {
        //qwtRenderBackground( painter, rect, widget );
    }

    const QwtLegendItem *item = qobject_cast<const QwtLegendItem *>( widget );
    if ( item )
    {
        const QRect identifierRect(
            rect.x() + item->margin(), rect.y(),
            item->identifierSize().width(), rect.height() );

        QwtLegendItemManager *itemManger = plot->legend()->find( item );
        if ( itemManger )
        {
            painter->save();
            itemManger->drawLegendIdentifier( painter, identifierRect );
            painter->restore();
        }

        // Label

        QRectF titleRect = rect;
        titleRect.setX( identifierRect.right() + 2 * item->spacing() );

        painter->setFont( item->font() );
        item->text().draw( painter, titleRect );
    }
}

/*!
  \brief Paint a scale into a given rectangle.
  Paint the scale into a given rectangle.

  \param plot Plot widget
  \param painter Painter
  \param axisId Axis
  \param baseDist Base distance
  \param rect Bounding rectangle
*/
void QwtPlotRenderer::renderScale( const QwtPlot *plot,
    QPainter *painter,
    int axisId, int baseDist, const QRectF &rect ) const
{
    if ( !plot->axisEnabled( axisId ) )
        return;

    const QwtScaleWidget *scaleWidget = plot->axisWidget( axisId );
    if ( scaleWidget->isColorBarEnabled()
        && scaleWidget->colorBarWidth() > 0 )
    {
        scaleWidget->drawColorBar( painter, scaleWidget->colorBarRect( rect ) );

        const int off = scaleWidget->colorBarWidth() + scaleWidget->spacing();
        if ( scaleWidget->scaleDraw()->orientation() == Qt::Horizontal )
            baseDist += off;
        else
            baseDist += off;
    }

    painter->save();

    QwtScaleDraw::Alignment align;
    double x, y, w;

    switch ( axisId )
    {
        case QwtPlot::yLeft:
        {
            x = rect.right() - 1.0 - baseDist;
            y = rect.y();
            w = rect.height();
            align = QwtScaleDraw::LeftScale;
            break;
        }
        case QwtPlot::yRight:
        {
            x = rect.left() + baseDist;
            y = rect.y();
            w = rect.height();
            align = QwtScaleDraw::RightScale;
            break;
        }
        case QwtPlot::xTop:
        {
            x = rect.left();
            y = rect.bottom() - 1.0 - baseDist;
            w = rect.width();
            align = QwtScaleDraw::TopScale;
            break;
        }
        case QwtPlot::xBottom:
        {
            x = rect.left();
            y = rect.top() + baseDist;
            w = rect.width();
            align = QwtScaleDraw::BottomScale;
            break;
        }
        default:
            return;
    }

    scaleWidget->drawTitle( painter, align, rect );

    painter->setFont( scaleWidget->font() );

    QwtScaleDraw *sd = const_cast<QwtScaleDraw *>( scaleWidget->scaleDraw() );
    const QPointF sdPos = sd->pos;
    const double sdLength = sd->len;

    sd->move( x, y );
    sd->setLength( w );

    QPalette palette = scaleWidget->palette();
    palette.setCurrentColorGroup( QPalette::Active );
    sd->draw( painter, palette );

    // reset previous values
    sd->move( sdPos );
    sd->setLength( sdLength );

    painter->restore();
}

/*!
  Render the canvas into a given rectangle.

  \param plot Plot widget
  \param painter Painter
  \param map Maps mapping between plot and paint device coordinates
  \param canvasRect Canvas rectangle
*/
void QwtPlotRenderer::renderCanvas( const QwtPlot *plot,
    QPainter *painter, const QRectF &canvasRect, 
    const QwtScaleMap *map ) const
{
    painter->save();

    qwtRenderBackground( painter, canvasRect, plot->canvas() );

    painter->restore();

    painter->save();

    int fw = plot->canvas()->frameWidth();
    painter->setClipRect( canvasRect.adjusted(fw, fw, -fw, -fw) );

    plot->drawItems( painter, canvasRect, map );

    painter->restore();
}

/*!
   Calculated the scale maps for rendering the canvas

   \param plot Plot widget
   \param canvasRect Target rectangle
   \param maps Scale maps to be calculated
*/
void QwtPlotRenderer::buildCanvasMaps( const QwtPlot *plot,
    const QRectF &canvasRect, QwtScaleMap maps[] ) const
{
    for ( int axisId = 0; axisId < QwtPlot::axisCnt; axisId++ )
    {
        maps[axisId].setTransformation(
            plot->axisScaleEngine( axisId )->transformation() );

        const QwtScaleDiv &scaleDiv = *plot->axisScaleDiv( axisId );
        maps[axisId].setScaleInterval(
            scaleDiv.lowerBound(), scaleDiv.upperBound() );

        double from, to;
        if ( plot->axisEnabled( axisId ) )
        {
            const QRectF &scaleRect = plot->plotLayout()->scaleRect( axisId );

            if ( axisId == QwtPlot::xTop || axisId == QwtPlot::xBottom )
            {
                from = scaleRect.left();
                to = scaleRect.right() + 1;
            }
            else
            {
                from = scaleRect.bottom() + 1;
                to = scaleRect.top();
            }
        }
        else
        {
            if ( axisId == QwtPlot::yLeft || axisId == QwtPlot::yRight )
            {
                from = canvasRect.bottom();
                to = canvasRect.top();
            }
            else
            {
                from = canvasRect.left();
                to = canvasRect.right();
            }
        }
        maps[axisId].setPaintInterval( from, to );
    }
}
