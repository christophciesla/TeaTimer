#include <QtGui>

#include "window/window.h"

int main(int argc, char* argv[])
{
	QApplication app{ argc, argv };
	app.setApplicationName("Tea timer");

	Window window{};
	window.show();

	return app.exec();
}
