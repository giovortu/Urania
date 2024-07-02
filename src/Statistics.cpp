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

void Statistics::setNumDigital( int num )
{
    ui->numDigital->setText( QString::number(num) );
}

void Statistics::setAllNumBooks(int num)
{
    ui->numBooks_2->setText( QString::number(num) );
}


void Statistics::setAllOwnedBooks(int num)
{
    ui->numOwned_2->setText( QString::number(num) );
}

void Statistics::setAllReadBooks(int num)
{
    ui->numRead_2->setText( QString::number(num) );
}

void Statistics::setAllNumDigital(int num)
{
    ui->numDigital_2->setText( QString::number(num) );

}

