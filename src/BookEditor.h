#ifndef BOOKEDITOR_H
#define BOOKEDITOR_H

#include <QWidget>
#include "ui_BookEditor.h"  // Generated UI header
#include "Book.h"           // Include the Book class definition

class Library;

namespace Ui {
class BookEditor;
}

class BookEditor : public QDialog
{
    Q_OBJECT

public:
    BookEditor(Library *library, QWidget* parent = nullptr);

    // Set the Book object to edit
    void setBook(Book* book);

private slots:
    // Handle user interaction and update the Book object
    void saveData();

    void loadCover();

    void addCollana();

    void populateCollana();

private:
    Ui::BookEditor ui;  // Generated UI object
    Book* book = nullptr;         // The Book object being edited
    Library *m_library = nullptr;
};


#endif // BOOKEDITOR_H
