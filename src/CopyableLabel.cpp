#include "CopyableLabel.h"
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QClipboard>

CopyableLabel::CopyableLabel(QWidget *parent): QLabel( parent)
{
    copyAction = new QAction("Copy", this);
    connect(copyAction, &QAction::triggered, this, &CopyableLabel::copyTextToClipboard);
}

CopyableLabel::CopyableLabel(const QString& text, QWidget* parent)
    : QLabel(text, parent)
{
    // Create a "Copy" action for the context menu
    copyAction = new QAction("Copy", this);
    connect(copyAction, &QAction::triggered, this, &CopyableLabel::copyTextToClipboard);
}

void CopyableLabel::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu menu(this);
    menu.addAction(copyAction);
    menu.exec(event->globalPos());
}

void CopyableLabel::copyTextToClipboard()
{
    QApplication::clipboard()->setText(this->text());
}
