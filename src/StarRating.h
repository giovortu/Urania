#ifndef STAR_RATING_H
#define STAR_RATING_H

#include <QWidget>

class StarRating : public QWidget
{
    Q_OBJECT

public:
    StarRating(QWidget *parent = nullptr);
    qreal getRating() const;
    void setRating(qreal rating);

signals:
    void ratingChanged(qreal rating);

protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;

private:
    qreal rating;
};

#endif // STARWIDGET_H
