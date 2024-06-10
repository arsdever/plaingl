#include <entt/entt.hpp>

#include "memory_manager.hpp"

#include "game_object.hpp"

memory_manager::memory_manager() = default;

memory_manager::~memory_manager() = default;

void memory_manager::initialize()
{
    if (!_instance)
    {
        _instance = new memory_manager;
    }
}

void memory_manager::deinitialize()
{
    if (_instance)
    {
        delete _instance;
        _instance = nullptr;
    }
}

memory_manager& memory_manager::instance()
{
    if (!_instance)
    {
        initialize();
    }
    return *_instance;
}

memory_manager* memory_manager::_instance = nullptr;
