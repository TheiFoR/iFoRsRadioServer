#ifndef TYPES_H
#define TYPES_H

#include <functional>
#include <QVariantMap>

using CallbackCommandFunction = std::function<void(const QVariantMap&)>;
using CallbackPacketFunction = std::function<void(const QString&, const QVariantMap&)>;

#endif // TYPES_H
