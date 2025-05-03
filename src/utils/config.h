#ifndef CONFIG_H
#define CONFIG_H

#include <QObject>
#include <QSettings>
#include <QString>
#include <QVariant>

class Config : public QObject
{
    Q_OBJECT
public:
    // Static methods to work with config
    static void setConfigFilePath(const QString &filePath);

    // Template getter and setter for any type with submodule support
    template <typename T>
    static T getValue(const QString &submodule, const QString &key, const T &defaultValue)
    {
        QSettings settings(m_configFilePath, QSettings::IniFormat);
        settings.beginGroup(submodule);

        if (!settings.contains(key)) {
            setValue(submodule, key, defaultValue);
        }

        T value = settings.value(key).value<T>();
        settings.endGroup();
        return value;
    }

    template <typename T>
    static void setValue(const QString &submodule, const QString &key, const T &value)
    {
        QSettings settings(m_configFilePath, QSettings::IniFormat);
        settings.beginGroup(submodule);

        settings.setValue(key, value);

        settings.sync();
        settings.endGroup();
    }

private:
    // Static member for file path with prefix
    static QString m_configFilePath;
};

#endif // CONFIG_H
