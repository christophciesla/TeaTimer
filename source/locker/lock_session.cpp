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
struct DBusLockMethod
{
    QString service{};
    QString path{};
    QString interface{};
};

ErrorCode LockSessionImpl_DBus()
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    if(!bus.isConnected())
    {
        return ErrorCode::SystemInaccessible;
    }
    QVector<DBusLockMethod> lock_methods{
        DBusLockMethod{"org.gnome.ScreenSaver", "/org/gnome/ScreenSaver", "org.gnome.ScreenSaver"},
        DBusLockMethod{"org.mate.ScreenSaver", "/", "org.mate.ScreenSaver"},
        DBusLockMethod{"org.xfce.ScreenSaver", "/", "org.xfce.Screensaver"},
        DBusLockMethod{"org.freedesktop.ScreenSaver", "/org/freedesktop/ScreenSaver", "org.freedesktop.ScreenSaver"}
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

ErrorCode LockSessionImpl_XScreenSaver()
{
    int return_value{QProcess::execute("xscreensaver-command", {"-lock"})};
    if (return_value >= 0)
    {
        return ErrorCode::Success;
    }
    return_value = QProcess::execute("xdg-screensaver", {"lock"});
    if (return_value >= 0)
    {
        return ErrorCode::Success;
    }
    return ErrorCode::DesktopEnvironmentNotSupported;
}

ErrorCode LockSessionImpl_Linux()
{
    const ErrorCode error_dbus{LockSessionImpl_DBus()};
    if (error_dbus == ErrorCode::Success)
    {
        return ErrorCode::Success;
    }
    const ErrorCode error_xscreensaver{LockSessionImpl_XScreenSaver()};
    if (error_xscreensaver == ErrorCode::Success)
    {
        return ErrorCode::Success;
    }
    return std::max(error_dbus, error_xscreensaver);
}
#endif
}

ErrorCode LockSession()
{
#ifdef WIN32
    ::LockWorkStation();
    return ErrorCode::Success;
#else
    return LockSessionImpl_Linux();
#endif
}
}
