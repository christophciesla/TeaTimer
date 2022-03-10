#include "locker/lock_session.h"

#ifdef WIN32
#include <Windows.h>
#else
#include <QtDBus>
#endif

namespace locker
{
namespace
{
#ifndef WIN32
struct LockMethod
{
    QString service{};
    QString path{};
    QString interface{};
};

ErrorCode LockSessionImpl_linux()
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    if(!bus.isConnected())
    {
        return ErrorCode::SystemInaccessible;
    }
    QVector<LockMethod> lock_methods{
        LockMethod{"org.gnome.ScreenSaver", "/org/gnome/ScreenSaver", "org.gnome.ScreenSaver"}
    };

    for(const auto& lock_method: lock_methods)
    {
        QDBusInterface screenSaverInterface(lock_method.service, lock_method.path,
                                            lock_method.interface, bus);
        if (screenSaverInterface.isValid())
        {
            std::ignore = screenSaverInterface.asyncCall("Lock");
            return ErrorCode::Success;
        }
    }
    return ErrorCode::DesktopEnvironmentNotSupported;
}
#endif
}

ErrorCode LockSession()
{
#ifdef WIN32
    ::LockWorkStation();
    return ErrorCode::Success;
#else
    return LockSessionImpl_linux();
#endif
}
}
