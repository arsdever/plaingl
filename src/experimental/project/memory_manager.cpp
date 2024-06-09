#include <entt/entt.hpp>

#include "memory_manager.hpp"

#include "game_object.hpp"

struct memory_manager::private_data
{
    entt::basic_registry<size_t> _registry;
};

memory_manager::memory_manager()
    : _pdata { std::make_unique<private_data>() }
{
}

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

template <>
std::shared_ptr<game_object> memory_manager::create<game_object>()
{
    auto object = std::shared_ptr<game_object>(new game_object);
    object->_id =
    instance()._pdata->_registry.create();
    return object;
}


memory_manager* memory_manager::_instance = nullptr;
