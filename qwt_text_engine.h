#pragma once

#include <qsize.h>

class QFont;
class QRectF;
class QString;
class QPainter;

/*!
  \brief A text engine for plain texts

  QwtPlainTextEngine renders texts using the basic Qt classes
  QPainter and QFontMetrics.
*/
class QwtPlainTextEngine
{
public:
    static double heightForWidth( const QFont &font, int flags,
        const QString &text, double width );

    static QSizeF textSize( const QFont &font, int flags,
        const QString &text );

    static void draw( QPainter *painter, const QRectF &rect,
        int flags, const QString &text );

    static void textMargins( const QFont &, const QString &,
        double &left, double &right, double &top, double &bottom );
};
