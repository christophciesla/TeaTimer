#include <QtGui>

#include "gui/window.h"

int main(int argc, char* argv[])
{
    QApplication app{ argc, argv };

    QTranslator translator{};
    const bool success{translator.load(QLocale(), "TeaTimer", "_", ":/translations")};
    if (success)
    {
        std::ignore = app.installTranslator(&translator);
    }
    app.setApplicationName(QApplication::tr("Tea timer"));

    gui::Window window{};
    window.show();

    return app.exec();
}
