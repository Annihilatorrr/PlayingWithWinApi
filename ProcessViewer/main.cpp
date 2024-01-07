#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "iwinprocessservice.h"
#include "winprocessservice.h"
#include <QFile>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/Icons/app.png"));
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "ProcessViewer_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    qDebug() << "Main thread " << QThread::currentThreadId();
    QFile styleSheetFile(":/mainstylesheet");
    styleSheetFile.open(QFile::ReadOnly);
    QString styleSheet = styleSheetFile.readAll();
    a.setStyleSheet(styleSheet);
    WA::IWinProcessService* processService = new WA::WinProcessService();
    MainWindow w(processService);
    //MainWindow w(FakeProcessService{});
    w.show();
try {
        a.exec();
}
catch(...) {
        // launch crash dialog
}
}
