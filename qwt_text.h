#pragma once

#include <qstring.h>
#include <qsize.h>
#include <qfont.h>

class QColor;
class QPen;
class QBrush;
class QRectF;
class QPainter;

/*!
  \brief A class representing a text

  A QwtText is a text including a set of attributes how to render it.

  - Background\n
    A text might have a background, defined by a QPen and QBrush
    to improve its visibility.
  - Font\n
    A text might have an individual font.
  - Color\n
    A text might have an individual color.
  - Render Flags\n
    Flags from Qt::AlignmentFlag and Qt::TextFlag used like in
    QPainter::drawText.

  \sa QwtTextLabel
*/

class QwtText
{
public:
    /*!
      \brief Paint Attributes

      Font and color and background are optional attributes of a QwtText.
      The paint attributes hold the information, if they are set.
    */
    enum PaintAttribute
    {
        //! The text has an individual font.
        PaintUsingTextFont = 0x01,

        //! The text has an individual color.
        PaintUsingTextColor = 0x02,

        //! The text has an individual background.
        PaintBackground = 0x04
    };

    //! Paint attributes
    typedef QFlags<PaintAttribute> PaintAttributes;

    /*!
      \brief Layout Attributes
      The layout attributes affects some aspects of the layout of the text.
    */
    enum LayoutAttribute
    {
        /*!
          Layout the text without its margins. This mode is useful if a
          text needs to be aligned accurately, like the tick labels of a scale.
          If QwtTextEngine::textMargins is not implemented for the format
          of the text, MinimumLayout has no effect.
         */
        MinimumLayout = 0x01
    };

    //! Layout attributes
    typedef QFlags<LayoutAttribute> LayoutAttributes;

    QwtText( const QString & = QString::null );
    QwtText( const QwtText & );
    ~QwtText();

    QwtText &operator=( const QwtText & );

    bool operator==( const QwtText & ) const;
    bool operator!=( const QwtText & ) const;

    void setText( const QString & );
    QString text() const;

    bool isNull() const;
    bool isEmpty() const;

    void setFont( const QFont & );
    QFont font() const;

    QFont usedFont( const QFont & ) const;

    void setRenderFlags( int flags );
    int renderFlags() const;

    void setColor( const QColor & );
    QColor color() const;

    QColor usedColor( const QColor & ) const;

    void setBackgroundPen( const QPen & );
    QPen backgroundPen() const;

    void setBackgroundBrush( const QBrush & );
    QBrush backgroundBrush() const;

    void setPaintAttribute( PaintAttribute, bool on = true );
    bool testPaintAttribute( PaintAttribute ) const;

    void setLayoutAttribute( LayoutAttribute, bool on = true );
    bool testLayoutAttribute( LayoutAttribute ) const;

    double heightForWidth( double width, const QFont & = QFont() ) const;
    QSizeF textSize( const QFont & = QFont() ) const;

    void draw( QPainter *painter, const QRectF &rect ) const;

private:
    class PrivateData;
    PrivateData *d_data;

    class LayoutCache;
    LayoutCache *d_layoutCache;
};

//! \return text().isNull()
inline bool QwtText::isNull() const
{
    return text().isNull();
}

//! \return text().isEmpty()
inline bool QwtText::isEmpty() const
{
    return text().isEmpty();
}

Q_DECLARE_OPERATORS_FOR_FLAGS( QwtText::PaintAttributes )
Q_DECLARE_OPERATORS_FOR_FLAGS( QwtText::LayoutAttributes )
