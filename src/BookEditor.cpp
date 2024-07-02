#include <QFileDialog>
#include <QInputDialog>
#include <QFile>

#include "Library.h"
#include "BookEditor.h"
#include "Book.h"

BookEditor::BookEditor(Library *library, QWidget* parent) : QDialog(parent),m_library(library)

{
    ui.setupUi(this);


    // Connect the Save button's clicked signal to the slot that saves the data
    connect(ui.saveButton, &QPushButton::clicked, this, &BookEditor::saveData);

    connect(ui.coverButton, &QPushButton::clicked, this, &BookEditor::loadCover);

    connect( ui.closeButton, &QPushButton::clicked, this, &BookEditor::reject );

    connect( ui.addCollanaButton, &QPushButton::clicked, this, &BookEditor::addCollana );


    populateCollana();

    QStringList editors = m_library->getEditors();
    foreach( QString editor, editors )
    {
        ui.editorCombo->addItem(editor);
    }


}

void BookEditor::setBook( Book *book)
{
    this->book = book;

    // Update the UI with the Book object's data
    ui.title_itaEdit->setText(book->title_ita);
    ui.title_origEdit->setText(book->title_orig);
    ui.authorEdit->setText(book->author);
    ui.date_pubEdit->setDate(book->date_pub);
    ui.cover_authorEdit->setText(book->cover_author);
    ui.synopsisEdit->setText(book->synopsis);

    ui.coverEdit->setText(book->cover_image_path);
    ui.ownedCheckbox->setChecked(book->owned);
    ui.starsSpin->setValue(book->stars / 10);
    ui.commentEdit->setText(book->comment);
    ui.reprintCheckbox->setChecked(book->reprint);
    ui.readCheckbox->setChecked(book->read);
    ui.collanaCombo->setCurrentText(book->collana);
    ui.editorCombo->setCurrentText(book->editore);

    ui.isDigitalCheck->setChecked( book->isDigital );


}

void BookEditor::saveData()
{
    // Retrieve data from input fields and set the Book object's attributes
    book->title_ita = ui.title_itaEdit->text();
    book->title_orig = ui.title_origEdit->text();
    book->author = ui.authorEdit->text();
    book->date_pub = ui.date_pubEdit->date();
    book->cover_author = ui.cover_authorEdit->text();
    book->synopsis = ui.synopsisEdit->toPlainText();
    book->owned = ui.ownedCheckbox->isChecked();
    book->stars = ui.starsSpin->value() * 10;
    book->comment = ui.commentEdit->toPlainText();
    book->reprint = ui.reprintCheckbox->isChecked();
    book->read = ui.readCheckbox->isChecked();
    book->collana = ui.collanaCombo->currentText();
    book->editore = ui.editorCombo->currentText();

    book->isDigital = ui.isDigitalCheck->isChecked();

    accept();

    // After setting the data, you can save the Book object or perform any necessary actions.
}

void BookEditor::loadCover()
{
    QString img = QFileDialog::getOpenFileName(this, tr("Open Image"), QDir::currentPath(), tr("Image Files (*.jpg)"));

    QFile file( img );
    if ( file.open(QIODevice::ReadOnly) )
    {
        QFileInfo info( img );
        QByteArray bytes = file.readAll();

        book->cover_image = bytes;

        ui.coverEdit->setText(info.fileName());


    }

}

void BookEditor::addCollana()
{
    QString newCollana = QInputDialog::getText(this, tr("New Collana"), tr("Collana name:"));
    if ( !newCollana.isEmpty() )
    {
        populateCollana();
        ui.collanaCombo->addItem( newCollana );
        ui.collanaCombo->setCurrentText( newCollana );
    }
}

void BookEditor::populateCollana()
{
    ui.collanaCombo->clear();
    QStringList collane = m_library->getCollane();
    foreach( QString collana, collane )
    {
        ui.collanaCombo->addItem(collana);
    }
}
