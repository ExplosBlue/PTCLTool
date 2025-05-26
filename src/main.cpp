#include "editor/mainWindow.h"

#include <QApplication>

#include "etc1/rg_etc1.h"


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    rg_etc1::pack_etc1_block_init();

    QCoreApplication::setOrganizationName("Explos");
    QCoreApplication::setApplicationName("PTCLEditor");

    PtclEditor::MainWindow win;
    win.show();

    return app.exec();
}
