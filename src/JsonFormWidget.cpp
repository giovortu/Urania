#include "JsonFormWidget.h"
#include <QScrollArea>

JsonFormWidget::JsonFormWidget(QWidget* parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::Window );//| Qt::WindowStaysOnTopHint);

    formLayout = new QFormLayout(this);

    auto vbox = new QVBoxLayout(this);
    setLayout( vbox );

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);

    QWidget* formWidget = new QWidget(this);
    formWidget->setLayout(formLayout);

    scrollArea->setWidget(formWidget);

}

void JsonFormWidget::loadJson(const QJsonObject& jsonObject)
{
    while (QLayoutItem* item = formLayout->takeAt(0)) {
        if (QWidget* widget = item->widget()) {
            delete widget;
        }
        delete item;
    }

    displayLabels.clear();
    createFormFields(jsonObject);
}

void JsonFormWidget::createFormFields(const QJsonObject& jsonObject, const QString& parentKey)
{
    for (const QString& key : jsonObject.keys()) {
        QJsonValue value = jsonObject.value(key);

        // Construct the full key path, including the parent's key (if any)
        QString fullKey = parentKey.isEmpty() ? key : parentKey + "." + key;

        if (value.isObject()) {
            createFormFields(value.toObject(), fullKey);
        } else if (value.isArray()) {
            // Handle arrays if needed
            // You can create multiple QLabel widgets for array items
        } else {
            // Create a QLabel for display
            QLabel* label = new QLabel(value.toString(), this);
            displayLabels[fullKey] = label;

            formLayout->addRow(key, label);
        }
    }
}
