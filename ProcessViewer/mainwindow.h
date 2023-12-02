#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets/QPushButton>
#include <QMenuBar>
#include <QStatusBar>
#include <QFutureWatcher>
#include <QCoreApplication>
#include <QtWidgets/QTreeView>
#include "treemodel.h"
#include "iprocessservice.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QFuture<std::map<unsigned int, ProcessInfo>> currentProcessesResultFuture;
    QTimer* _processInfoTimer;
    const IProcessService& _processService;
public:
    MainWindow(const IProcessService& processService, QWidget *parent = nullptr);
    std::map<unsigned int, ProcessInfo> getProcesses();
    QWidget *centralwidget;
    QPushButton *pushButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;
    QTreeView* treeView;
    TreeModel* model;
    void setupUi();
    void retranslateUi();
    ~MainWindow();
    void OnProcessInfoReceived(QFutureWatcher<std::map<unsigned int, ProcessInfo>>* processesStateWatcher);

private:
public slots:
};
#endif // MAINWINDOW_H
