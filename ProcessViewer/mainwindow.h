#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFuture>
#include "processinfo.h"



class QTreeView;

class ProcessTreeFilterModel;

class QTableView;
class QHBoxLayout;
class QLineEdit;
class ProcessTreeModel;
class FilterByComboBoxModel;
class QComboBox;
class ProcessTableModel;
class ProcessTableFilterModel;
namespace WA
{
    class IWinProcessService;
}

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
    FilterByComboBoxModel* m_filterByComboboxModel;
    QAction* m_switchToTableModeAction;
    QAction* m_switchToTreeModeAction;
    QLineEdit* m_filterLineEdit;
    QHBoxLayout* m_filterLayout;
    QComboBox* m_filterComboBox;
    QList<QPair<int,QString>>* m_filterByValues;
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
    QLineEdit* createFilterWidget();
    void headerResized(int index, int old, int news);
    void configureTableHorizontalHeader();

    void configureTreeViewHeader();

    void configureTreeView();

private:
    void connectSignalsAndSlots();

public slots:
};
#endif // MAINWINDOW_H
