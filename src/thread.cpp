#ifdef WIN32
/* clang-format off */
#include <windows.h>
#include <processthreadsapi.h>
/* clang-format on */
#else
#    include <pthread.h>
#endif

#include "thread.hpp"

void set_thread_name(std::thread& thd, std::string_view name)
{
#ifdef WIN32
    typedef HRESULT(WINAPI * SetThreadDescription)(HANDLE hThread,
                                                   PCWSTR lpThreadDescription);

    std::wstring wname = std::wstring(name.begin(), name.end());

    auto pfSetThreadDescription =
        reinterpret_cast<SetThreadDescription>(::GetProcAddress(
            ::GetModuleHandleA("KernelBase.dll"), "SetThreadDescription"));

    if (pfSetThreadDescription)
        pfSetThreadDescription(thd.native_handle(), wname.c_str());
#elif defined(__APPLE__)
    // TODO: find a solution for MacOSX
    return;
#else
    pthread_setname_np(thd.native_handle(), name.data());
#endif
}
