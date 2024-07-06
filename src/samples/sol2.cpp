#include <sol/sol.hpp>

int main(int argc, char* argv[])
{
    sol::state lua;
    int x = 0;
    lua.set_function("beep", [ &x ] { ++x; });
    lua.script("beep()");
    assert(x == 1);
    return 0;
}
