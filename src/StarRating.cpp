#include "StarRating.h"
#include <QPainter>
#include <QMouseEvent>

StarRating::StarRating(QWidget *parent) : QWidget(parent), rating(0) {}

qreal StarRating::getRating() const
{
    return rating;
}

void StarRating::setRating(qreal newRating)
{
    if (newRating != rating)
    {
        rating = qBound(0.0, newRating, 5.0); // Ensure rating is between 0 and 5
        update(); // Redraw the widget
    }
}

void StarRating::paintEvent(QPaintEvent *event)
{
#if 0
    Q_UNUSED(event);
    QPainter painter(this);
    for (int i = 0; i < 5; ++i)
    {
        qreal starRating = i + 0.5; // Add 0.5 for half-stars
        painter.drawPixmap(i * 20, 0, 20, 20, QPixmap((starRating <= rating) ? ":/images/star-filled.svg" : (starRating - 0.5 <= rating) ? ":/images/star-half.svg" : ":/images/star-empty.png"));
    }
#else
    Q_UNUSED(event);
    QPainter painter(this);
    qreal fullStars = static_cast<int>(rating);
    qreal halfStar = rating - fullStars;

    // Draw full stars
    for (int i = 0; i < fullStars; ++i) {
        painter.drawPixmap(i * 20, 0, 20, 20, QPixmap(":/images/star-filled.svg"));
    }

    // Draw half-star, if applicable
    if (halfStar > 0) {
        painter.drawPixmap(static_cast<int>(fullStars) * 20, 0, 20, 20, QPixmap(":/images/star-half.svg"));
    }

    // Draw empty stars for the remaining space
    for (int i = static_cast<int>(fullStars) + (halfStar > 0 ? 1 : 0); i < 5; ++i) {
        painter.drawPixmap(i * 20, 0, 20, 20, QPixmap(":/images/star-empty.svg"));
    }
#endif

}

void StarRating::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        qreal newRating = static_cast<qreal>(event->x()) / 20.0;
        setRating(newRating);
        emit ratingChanged(rating);
    }
}
