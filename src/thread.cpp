/* clang-format off */
#include <windows.h>
#include <processthreadsapi.h>
/* clang-format on */

#include "thread.hpp"

void set_thread_name(std::thread& thd, std::string_view name)
{
    typedef HRESULT(WINAPI * SetThreadDescription)(HANDLE hThread,
                                                   PCWSTR lpThreadDescription);

    std::wstring wname = std::wstring(name.begin(), name.end());

    auto pfSetThreadDescription =
        reinterpret_cast<SetThreadDescription>(::GetProcAddress(
            ::GetModuleHandleA("KernelBase.dll"), "SetThreadDescription"));

    if (pfSetThreadDescription)
        pfSetThreadDescription(thd.native_handle(), wname.c_str());
}

void set_thread_priority(std::thread& thd, int priority)
{
    SetThreadPriority(thd.native_handle(), priority);
}

adjust_timeout_accuracy_guard::adjust_timeout_accuracy_guard()
{
    timeBeginPeriod(1);
}

adjust_timeout_accuracy_guard::~adjust_timeout_accuracy_guard()
{
    timeEndPeriod(1);
}
