#include "editor/mainWindow.h"

#include <QApplication>


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("Explos");
    QCoreApplication::setApplicationName("PTCLTool");

    PtclTool::MainWindow win;
    win.show();

    return app.exec();
}
