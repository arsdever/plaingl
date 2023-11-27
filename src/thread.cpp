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
