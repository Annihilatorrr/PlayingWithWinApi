#include <QLayout>
#include <QComboBox>
#include <QtConcurrent/QtConcurrent>
#include <QToolbar>
#include <QLabel>
#include <QHeaderView>
#include <QSizePolicy>
#include <QTreeView>
#include <QTableView>
#include <process.h>
#include <ranges>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QLineEdit>

#include "mainwindow.h"
#include "iwinprocessservice.h"
#include "processtreeitem.h"
#include "processtreemodel.h"
#include "processtreefiltermodel.h"
#include "processserviceexception.h"
#include "filterbycomboboxmodel.h"
#include "processpropertiesdialog.h"
#include "processtablefiltermodel.h"
#include "processtablemodel.h"
MainWindow::MainWindow(WA::IWinProcessService* processService, QWidget *parent)
    : QMainWindow(parent), _processService(processService)
{
    setupUi();
}

std::map<unsigned int, ProcessInfo> MainWindow::getProcesses()
{
    std::map<unsigned int, ProcessInfo> processes = _processService->getProcessTreeByCom();

    //    auto range{ *processes | std::views::values };
    //    std::vector sortedProcesses(range.begin(), range.end());
    //    std::ranges::sort(sortedProcesses,
    //                      [](const auto& a, const auto& b)
    //                      {
    //                          return a.id > b.id;
    //                      });
    //    std::vector<ProcessInfo> result;
    //    for(auto elem : processes)
    //        result.push_back(elem.second);
    return processes;
}

void MainWindow::connectSignalsAndSlots()
{
    connect(m_treeView, &QTreeView::customContextMenuRequested, this, &MainWindow::onProcessTreeContextMenu);
    connect(m_tableView, &QTableView::customContextMenuRequested, this, &MainWindow::onProcessTableContextMenu);
    connect(m_processInfoTimer, &QTimer::timeout, this, [this]()
            {
                auto processesStateWatcher = new QFutureWatcher<std::map<unsigned int, ProcessInfo>>();
                currentProcessesResultFuture = QtConcurrent::run(&MainWindow::getProcesses, this);
                std::unique_ptr<QObject> context{new QObject};
                QObject* pcontext = context.get();
                connect(processesStateWatcher, &QFutureWatcher<std::map<unsigned int, ProcessInfo>>::finished,
                        pcontext, [=, this, context = std::move(context)]() mutable
                        {
                            try
                            {
                                OnProcessInfoReceived(processesStateWatcher);
                                context.release();
                            }
                            catch(ProcessServiceException& exc)
                            {
                                context.release();
                                qDebug() << "exception thrown while receiving list of processes";
                                qDebug() << "processesStateWatcher diconnected";
                            }
                        });
                connect(processesStateWatcher, &QFutureWatcher<std::map<unsigned int, ProcessInfo>>::finished, processesStateWatcher, &QFutureWatcher<std::map<unsigned int, ProcessInfo>>::deleteLater);
                processesStateWatcher->setFuture(currentProcessesResultFuture);
            });
}

void MainWindow::configureTableHorizontalHeader()
{
    auto tableHorizontalHeader = m_tableView->horizontalHeader();
    // tableHorizontalHeader->resizeSection(0, 170);
    // tableHorizontalHeader->resizeSection(1, 50);
    // tableHorizontalHeader->resizeSection(2, 100);
    // tableHorizontalHeader->resizeSection(3, 100);
    // tableHorizontalHeader->resizeSection(4, 200);
    // tableHorizontalHeader->resizeSection(5, 70);
    tableHorizontalHeader->setSectionsClickable(true);
}

void MainWindow::configureTreeViewHeader()
{
    auto treeViewHeader = m_treeView->header();
    // treeViewHeader->setSectionsClickable(true);
    // treeViewHeader->resizeSection(0, 170);
    // treeViewHeader->resizeSection(1, 50);
    // treeViewHeader->resizeSection(2, 100);
    // treeViewHeader->resizeSection(3, 100);
    // treeViewHeader->resizeSection(4, 200);
    // treeViewHeader->resizeSection(5, 70);
}

void MainWindow::configureTreeView()
{
    m_treeView->setModel(m_processTreeProxyModel);
    m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_treeView->setSortingEnabled(true);
    m_treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_treeView->setVisible(m_displayAsTree);
}

void MainWindow::setupUi()
{
    setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
    //setFixedSize(1024, 768);
    // setMaximumSize(1024, 768);
    setMinimumSize(1024, 768);
    // setWindowFlags(windowFlags()| Qt::MSWindowsFixedSizeDialogHint);
     m_treeView = new QTreeView();
    m_tableView = new QTableView();
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_switchToTableModeAction = new QAction();
    m_switchToTableModeAction->setIcon(QIcon(":/Icons/list-view.png"));

    m_switchToTableModeAction->setToolTip(QCoreApplication::translate("MainWindow", "ListMode", nullptr));

    m_switchToTreeModeAction = new QAction();
    m_switchToTreeModeAction->setIcon(QIcon(":/Icons/tree-view.png"));

    m_filterLayout = new QHBoxLayout();
    connect(m_switchToTableModeAction, &QAction::triggered, [this]()
            {
                m_displayAsTree = false;
                m_treeView->setVisible(m_displayAsTree);
                m_tableView->setVisible(!m_displayAsTree);
            });
    connect(m_switchToTreeModeAction, &QAction::triggered, [this]()
            {
                m_displayAsTree = true;
                m_treeView->setVisible(m_displayAsTree);
                m_tableView->setVisible(!m_displayAsTree);
            });
    m_switchToTreeModeAction->setToolTip(QCoreApplication::translate("MainWindow", "TreeMode", nullptr));

    m_topToolbar = new QToolBar();
    m_topToolbar->setMovable(false);
    m_topToolbar->setContextMenuPolicy(Qt::PreventContextMenu);
    m_topToolbar->addAction(m_switchToTableModeAction);
    m_topToolbar->addAction(m_switchToTreeModeAction);
    addToolBar(m_topToolbar);

    m_centralwidget = new QWidget(this);
    m_centralwidget->setObjectName("centralwidget");
    QVBoxLayout* layout = new QVBoxLayout(m_centralwidget);

    m_treeViewModel = new ProcessTreeModel(this);

    m_processTreeProxyModel = new ProcessTreeFilterModel(this);
    m_processTreeProxyModel->setSourceModel(m_treeViewModel);

    configureTreeView();

    m_tableView->setWordWrap(false);
    m_tableView->setContextMenuPolicy(Qt::CustomContextMenu);

    m_tableViewModel = new ProcessTableModel(this);
    configureTreeViewHeader();
    m_processTableProxyModel = new ProcessTableFilterModel(this);
    m_processTableProxyModel->setSourceModel(m_tableViewModel);

    QHeaderView* tableViewVerticalHeader = m_tableView->verticalHeader();

    tableViewVerticalHeader->setDefaultSectionSize(24);


    //    QWidget* paddingWidget = new QLabel();
    //    paddingWidget->setStyleSheet("QWidget { background-color : cyan; color : blue; }");
    //    paddingWidget->setMaximumWidth(10);

    m_filterByComboboxModel = new FilterByComboBoxModel();

    m_filterByValues = new QList<QPair<int,QString>>();

    m_filterByValues->append(QPair<int,QString>(-1,QCoreApplication::translate("MainWindow", "Everywhere", nullptr)));
    m_filterByValues->append(QPair<int,QString>(static_cast<int>(ProcessTableModel::Properties::ProcessName),QCoreApplication::translate("MainWindow", "ByName", nullptr)));
    m_filterByValues->append(QPair<int,QString>(static_cast<int>(ProcessTableModel::Properties::PID),QCoreApplication::translate("MainWindow", "ByPid", nullptr)));
    m_filterByValues->append(QPair<int,QString>(static_cast<int>(ProcessTableModel::Properties::ExecutablePath),QCoreApplication::translate("MainWindow", "ByExecutionPath", nullptr)));

    m_filterByComboboxModel->populate(m_filterByValues);
    
    m_filterComboBox = new QComboBox();
    m_filterComboBox->setModel(m_filterByComboboxModel);

    m_filterLineEdit = createFilterWidget();
    m_filterLayout->setSpacing(5);
    m_filterLayout->addWidget(m_filterLineEdit, 3);
    m_filterLayout->addWidget(m_filterComboBox, 1);
    m_contextMenu = new QMenu();
    auto killProcessAction = new QAction(QCoreApplication::translate("MainWindow", "KillProcess", nullptr), m_contextMenu);
    auto propertiesAction = new QAction(QCoreApplication::translate("MainWindow", "Properties...", nullptr), m_contextMenu);
    connect(killProcessAction, &QAction::triggered, this, &MainWindow::onKillProcessMenuActionClicked);
    connect(propertiesAction, &QAction::triggered, this, &MainWindow::onPropertiesMenuActionClicked);

    m_contextMenu->addAction(killProcessAction);
    m_contextMenu->addAction(propertiesAction);

    m_tableView->setModel(m_processTableProxyModel);
    m_tableView->setSortingEnabled(true);

    // tableViewVerticalHeader->setSectionResizeMode(0, QHeaderView::Interactive);
    configureTableHorizontalHeader();

    layout->addLayout(m_filterLayout);
    layout->addWidget(m_treeView);
    layout->addWidget(m_tableView);

    m_tableView->setVisible(!m_displayAsTree);
    setCentralWidget(m_centralwidget);
    m_menuBar = new QMenuBar();
    m_menuBar->setObjectName("menubar");
    m_menuBar->setGeometry(QRect(0, 0, 800, 22));
    //setMenuBar(m_menuBar);
    m_statusBar = new QStatusBar(this);
    m_statusBar->setObjectName("statusbar");
    setStatusBar(m_statusBar);

    retranslateUi();

    QMetaObject::connectSlotsByName(this);

    m_processInfoTimer = new QTimer;
    m_processInfoTimer->setSingleShot(false);

    connectSignalsAndSlots();

    m_processInfoTimer->start(1000);
} // setupUi

QLineEdit* MainWindow::createFilterWidget()
{
    QLineEdit* widget = new QLineEdit();
    widget->setPlaceholderText(QCoreApplication::translate("MainWindow", "Search", nullptr));
    //widget->setValidator(new QRegExpValidator(QRegExp("[A-Za-z]{0,255}")));
    connect(widget, &QLineEdit::textChanged,[=](const QString & text)
            {
                if (m_displayAsTree)
                {
                    auto currentIndex{m_filterComboBox->currentIndex()};
                    auto filterBy = m_filterByValues->at(currentIndex).first;
                    m_processTreeProxyModel->setSearchText(text, filterBy);
                }
                else
                {
                    auto currentIndex{m_filterComboBox->currentIndex()};
                    auto filterBy = m_filterByValues->at(currentIndex).first;
                    m_processTableProxyModel->setSearchText(text,filterBy);
                }
            });
    return widget;
}
void MainWindow::onPropertiesMenuActionClicked()
{
    if (m_displayAsTree)
    {
        auto indexes = m_treeView->selectionModel()->selectedIndexes();
        for (const auto& proxyIndex: indexes)
        {
            auto index = m_processTreeProxyModel->mapToSource(proxyIndex);
            if (index.isValid() && index.column() == 0)
            {
                auto item = m_treeViewModel->getItemByIndex(index);
                ProcessProperties model;
                model.processId = item->getId();
                model.processName = item->getName();

                ProcessPropertiesDialog* propertiesDialog = new ProcessPropertiesDialog(model, this);
                QObject::connect(this, &QMainWindow::destroyed, propertiesDialog, &QWidget::deleteLater);
                propertiesDialog->show();
            }
        }
    }
    else
    {
        auto indexes = m_tableView->selectionModel()->selectedIndexes();
        for (const auto& proxyIndex: indexes)
        {
            auto index = m_processTableProxyModel->mapToSource(proxyIndex);
            if (index.isValid() && index.column() == 0)
            {
                auto item = m_tableViewModel->getChildAtRow(index.row());
                ProcessProperties model;
                model.processId = item->getId();
                model.processName = item->getName();

                ProcessPropertiesDialog* propertiesDialog = new ProcessPropertiesDialog(model, this);
                QObject::connect(this, &QMainWindow::destroyed, propertiesDialog, &QWidget::deleteLater);
                propertiesDialog->show();
            }
        }
    }
}
void MainWindow::onKillProcessMenuActionClicked()
{
    if (m_displayAsTree)
    {
        auto indexes = m_treeView->selectionModel()->selectedIndexes();
        for (const auto& proxyIndex: indexes)
        {
            auto index = m_processTreeProxyModel->mapToSource(proxyIndex);
            if (index.isValid() && index.column() == 0)
            {
                auto item = m_treeViewModel->getItemByIndex(index);
                qDebug() << item->getName() << "Killing process" << item->getId() << item->getName();
                _processService->kill(item->getId());
            }
        }
    }
    else
    {
        auto indexes = m_tableView->selectionModel()->selectedIndexes();
        for (const auto& proxyIndex: indexes)
        {
            auto index = m_processTableProxyModel->mapToSource(proxyIndex);
            if (index.isValid() && index.column() == 0)
            {
                auto item = m_tableViewModel->getChildAtRow(index.row());
                qDebug() << item->getName() << "Killing process" << item->getId() << item->getName();
                _processService->kill(item->getId());
            }
        }
    }
}

void MainWindow::onProcessTreeContextMenu(const QPoint &point)
{
    QModelIndex index = m_treeView->indexAt(point);
    if (index.isValid()) {
        auto coords = m_treeView->viewport()->mapToGlobal(point);
        m_contextMenu->exec(coords);
    }
}

void MainWindow::onProcessTableContextMenu(const QPoint &point)
{
    QModelIndex index = m_tableView->indexAt(point);
    if (index.isValid()) {
        auto coords = m_tableView->viewport()->mapToGlobal(point);
        m_contextMenu->exec(coords);
    }
}

void MainWindow::retranslateUi()
{

} // retranslateUi

MainWindow::~MainWindow()
{

}

void MainWindow::OnProcessInfoReceived(QFutureWatcher<std::map<unsigned int, ProcessInfo>>* processesStateWatcher)
{
    auto data = processesStateWatcher->result();
    qDebug() << "Result received, continues in thread " << QThread::currentThreadId();
    if (m_displayAsTree)
    {
        m_treeViewModel->load(data);
    }
    else
    {
        m_tableViewModel->load(data);
    }
}

