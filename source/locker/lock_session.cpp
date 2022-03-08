#include "locker/lock_session.h"

#ifdef WIN32
#include <Windows.h>
#endif

namespace locker
{
    void LockSession()
    {
#ifdef WIN32
        ::LockWorkStation();
#endif
    }
}
