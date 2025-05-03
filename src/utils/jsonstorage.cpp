#include "jsonstorage.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QDebug>

JsonStorage::JsonStorage(const QString& filename)
    : m_filename(filename)
{}

bool JsonStorage::writeValue(const QString& key, const QVariant& value) {
    QFile file(m_filename);
    QJsonObject obj;

    // Читаем старый файл
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        if (doc.isObject())
            obj = doc.object();
        file.close();
    }

    // Обновляем значение
    obj.insert(key, QJsonValue::fromVariant(value));

    // Записываем обратно
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QJsonDocument doc(obj);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
        return true;
    }

    return false;
}

bool JsonStorage::writeMap(const QVariantMap& map) {
    QFile file(m_filename);
    QJsonObject obj;

    // Читаем старый файл
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        if (doc.isObject())
            obj = doc.object();
        file.close();
    }

    // Обновляем значениями из map
    for (auto it = map.begin(); it != map.end(); ++it) {
        obj.insert(it.key(), QJsonValue::fromVariant(it.value()));
    }

    // Записываем обратно
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QJsonDocument doc(obj);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
        return true;
    }

    return false;
}

QVariantMap JsonStorage::readAll(bool createIfNotExist) const {
    QFile file(m_filename);
    QVariantMap result;

    if (!file.exists()) {
        if (createIfNotExist) {
            // Создаём пустой JSON-файл
            if (file.open(QIODevice::WriteOnly)) {
                QJsonDocument emptyDoc((QJsonObject()));
                file.write(emptyDoc.toJson(QJsonDocument::Indented));
                file.close();
            }
        }
        return result; // Пустой map
    }

    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        if (doc.isObject()) {
            result = doc.object().toVariantMap();
        }
        file.close();
    }

    return result;
}
