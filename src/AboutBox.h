/**********************************************************************
*
* FILENAME    :   AboutForm.h
* CLASS       :   AboutForm
*
* DESCRIPTION :
*
* AUTHOR      :    Giovanni Ortu ( giovortu )
* EMAIL       :    giovanni.ortu@abinsula.com
* CREATED     :    25 / 5 / 2021
*
**********************************************************************/

#ifndef ABOUTBOX_H
#define ABOUTBOX_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class AboutBox; }
QT_END_NAMESPACE

class AboutBox : public QDialog
{
    Q_OBJECT
public:
    explicit AboutBox(QWidget *parent = nullptr);
    virtual ~AboutBox() = default;

signals:

public slots:

    void setInfo( const QString & title,
                  const QString & acro,
                  const QString & version,
                  const QString & buildDate,
                  const QString & gitRevision);

protected:

    virtual void changeEvent(QEvent *e) override;

protected slots:

    void on_closeButton_clicked();

private:
    Ui::AboutBox *ui;

    QString m_template;

};

#endif // ABOUTBOX_H
