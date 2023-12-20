#ifndef BOOKEDITOR_H
#define BOOKEDITOR_H

#include <QWidget>
#include "ui_BookEditor.h"  // Generated UI header
#include "Book.h"           // Include the Book class definition

namespace Ui {
class BookEditor;
}

class BookEditor : public QDialog
{
    Q_OBJECT

public:
    BookEditor(QWidget* parent = nullptr);

    // Set the Book object to edit
    void setBook(Book* book);

private slots:
    // Handle user interaction and update the Book object
    void saveData();

    void loadCover();

private:
    Ui::BookEditor ui;  // Generated UI object
    Book* book;         // The Book object being edited
};


#endif // BOOKEDITOR_H
