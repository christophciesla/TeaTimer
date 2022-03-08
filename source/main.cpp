#include <QtGui>

#include "gui/window.h"

int main(int argc, char* argv[])
{
    QApplication app{ argc, argv };
    app.setApplicationName("Tea timer");

    gui::Window window{};
    window.show();

    return app.exec();
}
