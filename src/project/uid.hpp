#pragma once

struct uid
{
    size_t id;
    bool operator==(const uid& o) const { return id == o.id; }
};

namespace std
{
template <>
struct hash<uid>
{
    size_t operator()(const uid& uid) const { return uid.id; }
};
} // namespace std
