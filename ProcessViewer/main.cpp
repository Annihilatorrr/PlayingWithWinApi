#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "processservice.h"
#include "fakeprocessservice.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "ProcessViewer_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

MainWindow w(ProcessService{});
//    MainWindow w(FakeProcessService{});
    w.show();
try {
        a.exec();
}
catch(...) {
        // launch crash dialog
}
}
