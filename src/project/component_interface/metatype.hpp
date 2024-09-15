#pragma once

struct metatype
{
    size_t type_id;
    std::string name;
    metatype* base;

    bool can_cast(const metatype& other) const
    {
        if (type_id == other.type_id)
            return true;

        return base && base->can_cast(other);
    }
};
