#include "config.h"
#include <QDebug>

QString Config::m_configFilePath = "server_config.ini";  // Default path

void Config::setConfigFilePath(const QString &filePath)
{
    // Set the path for the config file
    m_configFilePath = filePath;
}
