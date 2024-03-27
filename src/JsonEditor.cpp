#include "JsonEditor.h"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QJsonDocument>
#include <QFile>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

JsonEditor::JsonEditor(const QJsonObject& json, QWidget *parent)
    : QDialog(parent), m_json(json)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setupUI();
}

JsonEditor::JsonEditor(const QString& filePath, QWidget *parent)
    : QDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QByteArray jsonData = file.readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
        if (!jsonDoc.isNull()) {
            m_json = jsonDoc.object();
            setupUI();
        } else {
            qWarning() << "Failed to parse JSON from file:" << filePath;
        }
        file.close();
    } else {
        qWarning() << "Failed to open file:" << filePath;
    }
}

void JsonEditor::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_formLayout = new QFormLayout();

    for (auto it = m_json.begin(); it != m_json.end(); ++it) {
          createForm(it.value(), it.key());
    }

    QPushButton *saveButton = new QPushButton("Save");
    connect(saveButton, &QPushButton::clicked, this, &JsonEditor::saveJson);

    m_mainLayout->addLayout(m_formLayout);
    m_mainLayout->addWidget(saveButton);

    setLayout(m_mainLayout);
}

void JsonEditor::createForm(QJsonValue value, const QString& keyPrefix)
{
    qWarning() << keyPrefix << value;
    if ( keyPrefix.startsWith("_") )
    {
        return;
    }

    if (value.isObject()) {
        QJsonObject obj = value.toObject();
        for (auto it = obj.begin(); it != obj.end(); ++it) {
            createForm(it.value(), keyPrefix + "." + it.key());
        }
    } else if (value.isArray()) {
        QJsonArray arr = value.toArray();
        for (int i = 0; i < arr.size(); ++i) {
            createForm(arr[i], keyPrefix + QString("[%1]").arg(i));
        }
    } else {
        QLabel *label = new QLabel(keyPrefix);
        QLineEdit *edit = new QLineEdit(value.toString());
        m_formLayout->addRow(label, edit);
    }
}

void JsonEditor::saveJson()
{
    /*
    for (int i = 0; i < m_formLayout->rowCount(); ++i) {
        QLabel *label = qobject_cast<QLabel*>(m_formLayout->itemAt(i, QFormLayout::LabelRole)->widget());
        QLineEdit *edit = qobject_cast<QLineEdit*>(m_formLayout->itemAt(i, QFormLayout::FieldRole)->widget());
        if (label && edit) {
            QStringList keys = label->text().split('.');
            QJsonObject* currentObj = &m_json;
            for (int j = 0; j < keys.size(); ++j) {
                QString key = keys[j];
                if (key.startsWith('[') && key.endsWith(']')) {
                    int index = key.mid(1, key.length() - 2).toInt();
                    if (currentObj->isArray() && index < currentObj->toArray().size()) {
                        if (j == keys.size() - 1) {
                            QJsonValue oldValue = (*currentObj)[index];
                            (*currentObj)[index] = edit->text();
                            qDebug() << "Changed value at index" << index << "from" << oldValue.toString() << "to" << edit->text();
                        } else {
                            currentObj = &((*currentObj)[index].toObject());
                        }
                    } else {
                        qDebug() << "Invalid index" << index << "in array" << keys.mid(0, j).join(".");
                        break;
                    }
                } else {
                    if (j == keys.size() - 1) {
                        QJsonValue oldValue = (*currentObj)[key];
                        (*currentObj)[key] = edit->text();
                        qDebug() << "Changed value at key" << key << "from" << oldValue.toString() << "to" << edit->text();
                    } else {
                        currentObj = &((*currentObj)[key].toObject());
                    }
                }
            }
        }
    }
    */
}

QJsonObject JsonEditor::getJson() const
{
    return m_json;
}
