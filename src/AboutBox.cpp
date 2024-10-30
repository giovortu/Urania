/**********************************************************************
*
* FILENAME    :   AboutForm.cpp
* CLASS       :   AboutForm
*
* DESCRIPTION :
*
* AUTHOR      :    Giovanni Ortu ( giovortu )
* EMAIL       :    giovanni.ortu@abinsula.com
* CREATED     :    25 / 5 / 2021
*
**********************************************************************/

#include "AboutBox.h"
#include <QStyle>
#include <QScreen>
#include "ui_AboutBox.h"

AboutBox::AboutBox(QWidget *parent) : QDialog(parent),
    ui( new Ui::AboutBox)
{
    ui->setupUi( this );

    setAttribute(Qt::WA_DeleteOnClose);

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = screen->geometry();


    setGeometry(

       QStyle::alignedRect(
           Qt::LeftToRight,
           Qt::AlignCenter,
           this->size(),
           screenGeometry
       ) );

       m_template = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
                    "<html>"
                    "<head>"
                    "<meta name=\"qrichtext\" content=\"1\" />"
                    "<style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style>"
                    "</head>"
                    "<body style=\" font-family:'Ibm plex sans'; font-size:10pt; font-weight:456; font-style:normal;\">"
                    "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
                    "<span style=\" font-size:large; font-weight:600;\">%1 ( %2 ) %3</span></p>"
                    "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
                    "Based on Qt %4<br /><br />Built on %5<br /><br /"
                    ">From revision "
                    "<span style=\" text-decoration: underline; color:#007af4;\">%6</span></a><br /><br />"
                    "Written by Giovanni Ortu. Copyright 2020-2024. All rights reserved.<br /><br />"
                    "The program is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.</p>"
                    "</body>"
                    "</html>";



}

void AboutBox::setInfo( const QString & title,
                        const QString & acro,
                        const QString & version,
                        const QString & buildDate,
                        const QString & gitRevision)
{
    ui->textEdit->setHtml( m_template
             .arg( title)
             .arg( acro )
             .arg( version )
             .arg( QT_VERSION_STR )
             .arg( buildDate)
             .arg( gitRevision ) );
}



void AboutBox::on_closeButton_clicked()
{
    accept();
}

void AboutBox::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    } else {
        QWidget::changeEvent(e);
    }
}
