#pragma once

class game_clock
{
public:
    static game_clock* init();
    static std::chrono::duration<double> absolute();
    static std::chrono::duration<double> delta();
    static std::chrono::duration<double> physics_delta();

    void frame();
    void physics_frame();

private:
    static game_clock* _instance;
    std::chrono::steady_clock::time_point _start;
    std::chrono::steady_clock::time_point _last_frame_time_point;
    std::chrono::steady_clock::time_point _last_physics_frame_time_point;
    std::chrono::duration<double> _last_frame_duration;
    std::chrono::duration<double> _last_physics_frame_duration;
};
