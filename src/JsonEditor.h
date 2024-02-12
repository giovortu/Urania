#ifndef JSONEDITOR_H
#define JSONEDITOR_H

#include <QDialog>
#include <QJsonObject>

class QVBoxLayout;
class QFormLayout;

class JsonEditor : public QDialog
{
    Q_OBJECT

public:
    explicit JsonEditor(const QJsonObject& json, QWidget *parent = nullptr);
    explicit JsonEditor(const QString& filePath, QWidget *parent = nullptr);

    QJsonObject getJson() const;

private slots:
    void saveJson();

private:
    void setupUI();
    void createForm(QJsonValue value, const QString& keyPrefix = "");

    QJsonObject m_json;
    QVBoxLayout *m_mainLayout;
    QFormLayout *m_formLayout;
};

#endif // JSONEDITOR_H
