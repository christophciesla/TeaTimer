#ifndef LOCKER_LOCK_SESSION_H
#define LOCKER_LOCK_SESSION_H

#include <cstdint>

namespace locker
{
enum class ErrorCode : std::uint8_t
{
    Success                        = 0U,
    DesktopEnvironmentNotSupported = 1U,
    SystemInaccessible             = 2U
};

ErrorCode LockSession();
}

#endif // LOCKER_LOCK_SESSION_H
