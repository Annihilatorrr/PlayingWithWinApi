#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets/QPushButton>
#include <QMenuBar>
#include <QStatusBar>
#include <QFutureWatcher>
#include <QCoreApplication>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTableView>
#include "processtreemodel.h"
#include "processfiltermodel.h"
#include "processtablemodel.h"
#include "iprocessservice.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QFuture<std::map<unsigned int, ProcessInfo>> currentProcessesResultFuture;
    QTimer* _processInfoTimer;
    WA::IWinProcessService* _processService;
    QMenu* m_contextMenu;
    QToolBar* m_topToolbar;
    bool m_displayAsTree = true;
public:
    MainWindow(WA::IWinProcessService* processService, QWidget *parent = nullptr);
    std::map<unsigned int, ProcessInfo> getProcesses();
    QWidget *centralwidget;
    QPushButton *pushButton;
    QMenuBar *menubar;
    QStatusBar *statusbar;
    QTreeView* m_treeView;
    QTableView* m_tableView;
    ProcessFilterModel* proxyModel;
    ProcessTreeModel* m_treeViewModel;
    ProcessTableModel* m_tableViewModel;
    QLineEdit* findByNameTextEdit;
    void setupUi();
    void retranslateUi();
    ~MainWindow();
    void onProcessTreeContextMenu(const QPoint &point);
    void OnProcessInfoReceived(QFutureWatcher<std::map<unsigned int, ProcessInfo>>* processesStateWatcher);
    void killProcessMenuActionClicked();
    void propertiesMenuActionClicked();
private:
public slots:
};
#endif // MAINWINDOW_H
