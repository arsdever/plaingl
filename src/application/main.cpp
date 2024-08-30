#include "application/application.hpp"
#include "common/logging.hpp"
#include "core/game_clock.hpp"

int main(int argc, char** argv)
{
    game_clock::init();
    configure_logging(argc, argv);

    application app;
    auto result = app.run();

    return result;
}
