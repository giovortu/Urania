#include "Statistics.h"
#include "ui_Statistics.h"

Statistics::Statistics(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Statistics)
{
    ui->setupUi(this);


    connect( ui->pushButton, &QPushButton::clicked, this, &Statistics::accept );
}

Statistics::~Statistics()
{
    delete ui;
}

void Statistics::setNumBooks(int num)
{
    ui->numBooks->setText( QString::number(num) );
}

void Statistics::setOwnedBooks(int num)
{
    ui->numOwned->setText( QString::number(num) );

}

void Statistics::setReadBooks(int num)
{
    ui->numRead->setText( QString::number(num) );
}

