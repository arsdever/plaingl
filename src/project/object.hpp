#pragma once

#include "project/uid.hpp"

/**
 * @brief Generic high level object representation
 */
class object
{
public:
    virtual ~object();

    std::string get_name() const;
    void set_name(const std::string& name);

    uid id() const;

protected:
    /**
     * @brief Construct a new object object
     *
     * The constructor is private, so only the memory manager can create a
     * new object
     */
    object();

public:
    friend class memory_manager;

private:
    uid _id;
    std::string _name;
};
