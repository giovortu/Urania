#ifndef STATISTICS_H
#define STATISTICS_H

#include <QDialog>

namespace Ui {
class Statistics;
}

class Statistics : public QDialog
{
    Q_OBJECT

public:
    explicit Statistics(QWidget *parent = nullptr);
    ~Statistics();

public slots:

    void setNumBooks( int num );
    void setOwnedBooks( int num );
    void setReadBooks( int num );



private:
    Ui::Statistics *ui;
};

#endif // STATISTICS_H
