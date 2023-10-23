#ifndef COPYABLELABEL_H
#define COPYABLELABEL_H

#include <QLabel>

class CopyableLabel : public QLabel
{
    Q_OBJECT

public:
    CopyableLabel(QWidget* parent = nullptr);
    CopyableLabel(const QString& text, QWidget* parent = nullptr);

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;

private slots:
    void copyTextToClipboard();

private:
    QAction* copyAction;
};

#endif // COPYABLELABEL_H
