#include <QApplication>
#include <QMainWindow>
#include <QThread>
#include <iostream>

#include <GLFW/glfw3.h>
#include <assets/asset_manager.hpp>
#include <common/main_thread_dispatcher.hpp>
#include <graphics/graphics_fwd.hpp>

#include "common/filesystem.hpp"
#include "tools/material_viewer/material_viewer.hpp"

int main(int argc, char** argv)
{
    int exit_code;
    QApplication* app;
    auto qt_thread = QThread::create([ &argc, argv, &exit_code ]()
    {
        QApplication app(argc, argv);
        QMainWindow window;
        window.show();
        exit_code = app.exec();
    });
    qt_thread->setObjectName("qt_gui_thread");
    qt_thread->start();

    common::main_thread_dispatcher::initialize();

    glfwInit();

    std::shared_ptr<material_viewer> viewer =
        std::make_shared<material_viewer>();
    viewer->on_user_initialize +=
        [ &app, &viewer ](std::shared_ptr<core::window> wnd)
    {
        assets::asset_manager::initialize(
            (common::filesystem::path::current_dir() / "resources")
                .full_path());
        assets::asset_manager::scan_project_directory();
        assets::asset_manager::setup_project_directory_watch();
        viewer->get_events()->close += [ &app ](auto ce) { app->quit(); };
        viewer->set_mesh_presets({
            assets::asset_manager::try_get<graphics::mesh>("meshes.cube.fbx"),
            assets::asset_manager::try_get<graphics::mesh>("meshes.sphere.fbx"),
            assets::asset_manager::try_get<graphics::mesh>("meshes.shader.fbx"),
        });
        viewer->set_material(assets::asset_manager::try_get<graphics::material>(
            "standard.standard.mat"));
    };
    viewer->init();

    while (!qt_thread->isFinished())
    {
        viewer->update();
        common::main_thread_dispatcher::run_all();
    }

    common::main_thread_dispatcher::shutdown();

    return 0;
}
