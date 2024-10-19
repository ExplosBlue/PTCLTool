#include "editor/mainwindow.h"

#include <QApplication>

int main(int argc, char* argv[]) {

    QApplication app(argc, argv);
    PtclEditor::MainWindow win;
    win.show();

    return app.exec();
}
