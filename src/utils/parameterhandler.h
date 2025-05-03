#ifndef PARAMETERHANDLER_H
#define PARAMETERHANDLER_H

#include <QVariantMap>
#include <QDebug>

class ParameterHandler
{
public:
    enum FieldStatus {
        Optional,
        Mandatory
    };

    ParameterHandler(const QVariantMap& parameters);

    template <FieldStatus fieldStatus = Mandatory, typename T>
    bool handle(T& variable, const QString& key);

private:
    const QVariantMap m_parameters;

    template <FieldStatus fieldStatus = Mandatory>
    bool isOptional(const QString& name, const char* msg);
};

template<ParameterHandler::FieldStatus fieldStatus, typename T>
bool ParameterHandler::handle(T& variable, const QString& key)
{
    auto iter = m_parameters.find(key);
    if (iter != m_parameters.end() && iter->canConvert<T>())
    {
        variable = iter->value<T>();
        return true;
    }
    return isOptional<fieldStatus>(key, "Mandatory field not found or conversion failed, field name: ");
}

template<ParameterHandler::FieldStatus fieldStatus>
bool ParameterHandler::isOptional(const QString& name, const char* msg)
{
    if (fieldStatus == Mandatory)
    {
        qWarning() << msg << name;
        return false;
    }
    return true;
}

#endif // PARAMETERHANDLER_H
