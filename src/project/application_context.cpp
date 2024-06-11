#include "project/application_context.hpp"

#include "project/memory_manager.hpp"

application_context::application_context() { memory_manager::initialize(); }

application_context::~application_context() { memory_manager::deinitialize(); }
