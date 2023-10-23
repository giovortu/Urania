#include "JsonFormWidget.h"
#include <QScrollArea>

JsonFormWidget::JsonFormWidget(QWidget* parent) : QWidget(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::Window );//| Qt::WindowStaysOnTopHint);

    formLayout = new QFormLayout(this);
    QScrollArea* scrollArea = new QScrollArea(this);
    QWidget* contentWidget = new QWidget(scrollArea);

    // Add your widgets and layout to the content widget here.

    // Set the content widget for the scroll area
    scrollArea->setWidget(contentWidget);
    scrollArea->setWidgetResizable(true);

    contentWidget->setLayout( formLayout );

    // Add the scroll area to your layout (e.g., QVBoxLayout)
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(scrollArea);
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
    formLayout->update();

}

void JsonFormWidget::displayJsonData(const QJsonObject& jsonObject, QLayout* layout)
{
    for (auto it = jsonObject.begin(); it != jsonObject.end(); ++it) {
        QString key = it.key();
        QJsonValue value = it.value();

        // Create a QLabel for the key
        QLabel* keyLabel = new QLabel(key);
        layout->addWidget(keyLabel);

        // Check the value's type
        if (value.isObject()) {
            // If the value is an object, recursively call the function
            QJsonObject nestedObject = value.toObject();
            QVBoxLayout* nestedLayout = new QVBoxLayout;
            layout->addItem(nestedLayout);
            displayJsonData(nestedObject, nestedLayout);
        } else if (value.isArray()) {
            // If the value is an array, recursively call the function for each item
            QJsonArray array = value.toArray();
            QVBoxLayout* arrayLayout = new QVBoxLayout;
            layout->addItem(arrayLayout);
            for (int i = 0; i < array.size(); ++i) {
                QJsonValue arrayValue = array[i];
                displayJsonData(arrayValue.toObject(), arrayLayout);
            }
        } else {
            // If it's a simple value, just display it as text
            QLabel* valueLabel = new QLabel(value.toString());
            layout->addWidget(valueLabel);
        }
    }
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
            label->setWordWrap( true );
            displayLabels[fullKey] = label;

            formLayout->addRow(key, label);
        }
    }
}
