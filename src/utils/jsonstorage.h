#ifndef JSONSTORAGE_H
#define JSONSTORAGE_H

#include <QString>
#include <QVariant>
#include <QVariantMap>

class JsonStorage {
public:
    explicit JsonStorage(const QString& filename);

    bool writeValue(const QString& key, const QVariant& value);
    bool writeMap(const QVariantMap& map);
    QVariantMap readAll(bool createIfNotExist = false) const;

private:
    QString m_filename;
};

#endif // JSONSTORAGE_H
