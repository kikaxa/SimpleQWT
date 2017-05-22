#pragma once

#include <qvector.h>
#include <qrect.h>

/*!
   \brief Abstract interface for iterating over samples

   Qwt offers several implementations of the QwtSeriesData API,
   but in situations, where data of an application specific format
   needs to be displayed, without having to copy it, it is recommended
   to implement an individual data access.
*/
template <typename T>
class QwtSeriesData
{
public:
    QwtSeriesData();
    virtual ~QwtSeriesData();

    //! \return Number of samples
    virtual int size() const = 0;

    /*!
      Return a sample
      \param i Index
      \return Sample at position i
     */
    virtual T sample( int i ) const = 0;

    /*!
       Calculate the bounding rect of all samples

       The bounding rect is necessary for autoscaling and can be used
       for a couple of painting optimizations.

       qwtBoundingRect(...) offers slow implementations iterating
       over the samples. For large sets it is recommended to implement
       something faster f.e. by caching the bounding rect.
     */
    virtual QRectF boundingRect() const = 0;

protected:
    //! Can be used to cache a calculated bounding rectangle
    mutable QRectF d_boundingRect;

private:
    QwtSeriesData<T> &operator=( const QwtSeriesData<T> & );
};

//! Constructor
template <typename T>
QwtSeriesData<T>::QwtSeriesData():
    d_boundingRect( 0.0, 0.0, -1.0, -1.0 )
{
}

//! Destructor
template <typename T>
QwtSeriesData<T>::~QwtSeriesData()
{
}

/*!
  \brief Template class for data, that is organized as QVector

  QVector uses implicit data sharing and can be
  passed around as argument efficiently.
*/
template <typename T>
class QwtArraySeriesData: public QwtSeriesData<T>
{
public:
    QwtArraySeriesData();
    QwtArraySeriesData( const QVector<T> & );

    void setSamples( const QVector<T> & );
    const QVector<T> samples() const;

    virtual int size() const;
    virtual T sample( int ) const;

protected:
    //! Vector of samples
    QVector<T> d_samples;
};

//! Constructor
template <typename T>
QwtArraySeriesData<T>::QwtArraySeriesData()
{
}

/*!
   Constructor
   \param samples Array of samples
*/
template <typename T>
QwtArraySeriesData<T>::QwtArraySeriesData( const QVector<T> &samples ):
    d_samples( samples )
{
}

/*!
  Assign an array of samples
  \param samples Array of samples
*/
template <typename T>
void QwtArraySeriesData<T>::setSamples( const QVector<T> &samples )
{
    QwtSeriesData<T>::d_boundingRect = QRectF( 0.0, 0.0, -1.0, -1.0 );
    d_samples = samples;
}

//! \return Array of samples
template <typename T>
const QVector<T> QwtArraySeriesData<T>::samples() const
{
    return d_samples;
}

//! \return Number of samples
template <typename T>
int QwtArraySeriesData<T>::size() const
{
    return d_samples.size();
}

/*!
  Return a sample
  \param i Index
  \return Sample at position i
*/
template <typename T>
T QwtArraySeriesData<T>::sample( int i ) const
{
    return d_samples[(int)i];
}

//! Interface for iterating over an array of points
class QwtPointSeriesData: public QwtArraySeriesData<QPointF>
{
public:
    QwtPointSeriesData(
        const QVector<QPointF> & = QVector<QPointF>() );

    virtual QRectF boundingRect() const;
};

/*!
  \brief Interface for iterating over two QVector<double> objects.
*/
class QwtPointArrayData: public QwtSeriesData<QPointF>
{
public:
    QwtPointArrayData( const QVector<double> &x, const QVector<double> &y );
    QwtPointArrayData(const double *x, const double *y, int size );

    virtual QRectF boundingRect() const;

    virtual int size() const;
    virtual QPointF sample( int i ) const;

    const QVector<double> &xData() const;
    const QVector<double> &yData() const;

private:
    QVector<double> d_x;
    QVector<double> d_y;
};

/*!
  \brief Data class containing two pointers to memory blocks of doubles.
 */
class QwtCPointerData: public QwtSeriesData<QPointF>
{
public:
    QwtCPointerData( const double *x, const double *y, int size );

    virtual QRectF boundingRect() const;
    virtual int size() const;
    virtual QPointF sample(int i ) const;

    const double *xData() const;
    const double *yData() const;

private:
    const double *d_x;
    const double *d_y;
    int d_size;
};

QRectF qwtBoundingRect(
    const QwtSeriesData<QPointF> &, int from = 0, int to = -1 );
