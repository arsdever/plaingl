#pragma once

#include "experimental/project/object.hpp"

class game_object : public object
{
public:
    static std::shared_ptr<game_object> create();

private:
    friend class memory_manager;
    game_object();
};
