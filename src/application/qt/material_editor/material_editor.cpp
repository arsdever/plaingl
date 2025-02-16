#include <QApplication>
#include <QMainWindow>
#include <QOpenGLWidget>

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    QOpenGLWidget window;
    window.show();

    return app.exec();
}
