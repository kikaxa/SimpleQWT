/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_abstract_scale.h"
#include "qwt_scale_engine.h"
#include "qwt_scale_draw.h"
#include "qwt_scale_div.h"
#include "qwt_scale_map.h"
#include "qwt_interval.h"

/*!
  Constructor

  Creates a default QwtScaleDraw and a QwtLinearScaleEngine.
*/

QwtAbstractScale::QwtAbstractScale():
    stepSize( 0.0 )
{
    scaleEngine = new QwtLinearScaleEngine;
    scaleDraw = new QwtScaleDraw();
    rescale( 0.0, 100.0 );
}

//! Destructor
QwtAbstractScale::~QwtAbstractScale()
{
    delete scaleEngine;
    delete scaleDraw;
}

/*!
  \brief Specify a scale.

  Disable autoscaling and define a scale by an interval and a step size

  \param vmin lower limit of the scale interval
  \param vmax upper limit of the scale interval
  \param stepSize major step size
*/
void QwtAbstractScale::setScale( double vmin, double vmax, double _stepSize )
{
    stepSize = _stepSize;

    rescale( vmin, vmax, stepSize );
}

/*!
  \brief Specify a scale.

  Disable autoscaling and define a scale by an interval and a step size

  \param interval Interval
  \param stepSize major step size
*/
void QwtAbstractScale::setScale( const QwtInterval &interval, double stepSize )
{
    setScale( interval.minValue(), interval.maxValue(), stepSize );
}


/*!
  \brief Specify a scale.

  Disable autoscaling and define a scale by a scale division

  \param scaleDiv Scale division
*/
void QwtAbstractScale::setScale( const QwtScaleDiv &scaleDiv )
{
    if ( scaleDiv != scaleDraw->scaleDiv() )
    {
        scaleDraw->setScaleDiv( scaleDiv );
        scaleChange();
    }
}

/*!
  Recalculate the scale division and update the scale draw.

  \param vmin Lower limit of the scale interval
  \param vmax Upper limit of the scale interval
  \param stepSize Major step size

  \sa scaleChange()
*/
void QwtAbstractScale::rescale( double vmin, double vmax, double stepSize )
{
    const QwtScaleDiv scaleDiv = scaleEngine->divideScale(
        vmin, vmax, 5, 3, stepSize );

    if ( scaleDiv != scaleDraw->scaleDiv() )
    {
        scaleDraw->setTransformation(
            scaleEngine->transformation() );
        scaleDraw->setScaleDiv( scaleDiv );
        scaleChange();
    }
}

/*!
  \brief Set a scale draw

  scaleDraw has to be created with new and will be deleted in
  ~QwtAbstractScale or the next call of setAbstractScaleDraw.
*/
void QwtAbstractScale::setAbstractScaleDraw( QwtScaleDraw *_scaleDraw )
{
    if ( _scaleDraw == NULL || scaleDraw == _scaleDraw )
        return;

    if ( scaleDraw != NULL )
        _scaleDraw->setScaleDiv( scaleDraw->scaleDiv() );

    delete scaleDraw;
    scaleDraw = _scaleDraw;
}

void QwtAbstractScale::updateScaleDraw()
{
    rescale( scaleDraw->scaleDiv().lowerBound(),
        scaleDraw->scaleDiv().upperBound(), stepSize );
}

/*!
  \brief Set a scale engine

  The scale engine is responsible for calculating the scale division,
  and in case of auto scaling how to align the scale.

  scaleEngine has to be created with new and will be deleted in
  ~QwtAbstractScale or the next call of setScaleEngine.
*/
void QwtAbstractScale::setScaleEngine( QwtScaleEngine *_scaleEngine )
{
    if ( _scaleEngine != NULL && scaleEngine != _scaleEngine )
    {
        delete scaleEngine;
        scaleEngine = _scaleEngine;
    }
}

/*!
  \brief Notify changed scale

  Dummy empty implementation, intended to be overloaded by derived classes
*/
void QwtAbstractScale::scaleChange()
{
}

