#pragma once

class memory_manager
{
public:
    memory_manager();
    ~memory_manager();
    static void initialize();
    static void deinitialize();
    static memory_manager& instance();

    template <typename T>
    static std::shared_ptr<T> create();

private:
    static memory_manager* _instance;
    struct private_data;
    std::unique_ptr<private_data> _pdata;
};
