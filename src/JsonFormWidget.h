
#ifndef JSONFORMWIDGET_H
#define JSONFORMWIDGET_H


#include <QWidget>
#include <QFormLayout>
#include <QLabel>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QString>
#include <QDebug>

class JsonFormWidget : public QWidget
{
    Q_OBJECT

public:
    JsonFormWidget(QWidget* parent = nullptr);

    void loadJson(const QJsonObject& jsonObject);

private:
    QFormLayout* formLayout;
    QMap<QString, QLabel*> displayLabels;

    void createFormFields(const QJsonObject& jsonObject, const QString& parentKey = "");
};


#endif // JSONFORMWIDGET_H
