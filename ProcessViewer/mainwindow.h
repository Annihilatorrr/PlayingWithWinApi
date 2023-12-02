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
    WA::IWinProcessService* _processService;
    QMenu* m_contextMenu;
public:
    MainWindow(WA::IWinProcessService* processService, QWidget *parent = nullptr);
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
    void onProcessTreeContextMenu(const QPoint &point);
    void OnProcessInfoReceived(QFutureWatcher<std::map<unsigned int, ProcessInfo>>* processesStateWatcher);
    void uninstallAppletClickedSlot();
private:
public slots:
};
#endif // MAINWINDOW_H
