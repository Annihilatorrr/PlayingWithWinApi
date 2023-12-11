#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets/QPushButton>
#include <QMenuBar>
#include <QStatusBar>
#include <QFutureWatcher>
#include <QCoreApplication>
#include <QHBoxLayout>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QTableView>
#include "processtreemodel.h"
#include "processtablefiltermodel.h"
#include "processtreefiltermodel.h"
#include "processtablemodel.h"
#include "iprocessservice.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QFuture<std::map<unsigned int, ProcessInfo>> currentProcessesResultFuture;
    QTimer* m_processInfoTimer;
    WA::IWinProcessService* const _processService;
    QMenu* m_contextMenu;
    QToolBar* m_topToolbar;
    bool m_displayAsTree = true;
    QWidget* m_centralwidget;
    QMenuBar *m_menuBar;
    QStatusBar* m_statusBar;
    QTreeView* m_treeView;
    QTableView* m_tableView;
    ProcessTreeFilterModel* m_processTreeProxyModel;
    ProcessTableFilterModel* m_processTableProxyModel;
    ProcessTreeModel* m_treeViewModel;
    ProcessTableModel* m_tableViewModel;
    QAction* m_switchToTableModeAction;
    QAction* m_switchToTreeModeAction;
    std::map<int, QWidget*> m_filteringWidgets;
    QHBoxLayout* m_filterLayout;
public:
    MainWindow(WA::IWinProcessService* processService, QWidget *parent = nullptr);
    std::map<unsigned int, ProcessInfo> getProcesses();
    void setupUi();
    void retranslateUi();
    ~MainWindow();
    void onProcessTreeContextMenu(const QPoint &point);
    void onProcessTableContextMenu(const QPoint &point);
    void OnProcessInfoReceived(QFutureWatcher<std::map<unsigned int, ProcessInfo>>* processesStateWatcher);
    void onKillProcessMenuActionClicked();
    void onPropertiesMenuActionClicked();
    QWidget* createFilterWidgetForColumn(int i);
    void headerResized(int index, int old, int news);
    void configureTableHorizontalHeader();

    void configureTreeViewHeader();

    void configureTreeView();

private:
    void connectSignalsAndSlots();

public slots:
};
#endif // MAINWINDOW_H
