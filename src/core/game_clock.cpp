#include "game_clock.hpp"

game_clock* game_clock::init()
{
    if (_instance == nullptr)
    {
        _instance = new game_clock;
    }

    return _instance;
}

std::chrono::duration<double> game_clock::absolute()
{
    return _instance ? _instance->_last_frame_time_point - _instance->_start
                     : std::chrono::duration<double> {};
}

std::chrono::duration<double> game_clock::delta()
{
    return _instance ? _instance->_last_frame_duration
                     : std::chrono::duration<double> {};
}

std::chrono::duration<double> game_clock::physics_delta()
{
    return _instance ? _instance->_last_physics_frame_duration
                     : std::chrono::duration<double> {};
}

void game_clock::frame()
{
    auto now = std::chrono::steady_clock::now();
    _instance->_last_frame_duration = now - _instance->_last_frame_time_point;
    _instance->_last_frame_time_point = std::move(now);
}

void game_clock::physics_frame()
{
    auto now = std::chrono::steady_clock::now();
    _instance->_last_physics_frame_duration =
        now - _instance->_last_physics_frame_time_point;
    _instance->_last_physics_frame_time_point = std::move(now);
}

game_clock* game_clock::_instance = nullptr;
