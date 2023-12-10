#include "mainwindow.h"
#include "processtreeitem.h"
#include "processserviceexception.h"
#include "processpropertiesdialog.h"
#include <QLayout>
#include <QtConcurrent/QtConcurrent>
#include <QToolbar>
#include <QHeaderView>
#include <QSizePolicy>
#include <process.h>
#include <ranges>
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

void MainWindow::setupUi()
{
    m_topToolbar = new QToolBar();
    m_topToolbar->setContextMenuPolicy(Qt::PreventContextMenu);
    QAction* switchToListMode = new QAction();
    connect(switchToListMode, &QAction::triggered, [this]()
            {
                m_displayAsTree = false;
                m_treeView->setVisible(m_displayAsTree);
                m_tableView->setVisible(!m_displayAsTree);
            });
    switchToListMode->setToolTip(QCoreApplication::translate("MainWindow", "List mode", nullptr));
    QAction* switchToTreeMode = new QAction();
    connect(switchToTreeMode, &QAction::triggered, [this]()
            {
                m_displayAsTree = true;
                m_treeView->setVisible(m_displayAsTree);
                m_tableView->setVisible(!m_displayAsTree);
            });
    switchToTreeMode->setToolTip(QCoreApplication::translate("MainWindow", "Tree mode", nullptr));
    m_topToolbar->addAction(switchToListMode);
    m_topToolbar->addAction(switchToTreeMode);
    addToolBar(m_topToolbar);
    setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
    resize(800, 600);
    centralwidget = new QWidget(this);
    centralwidget->setObjectName("centralwidget");
    QVBoxLayout* layout = new QVBoxLayout(centralwidget);
    pushButton = new QPushButton();
    pushButton->setText(QCoreApplication::translate("MainWindow", "clickme", nullptr));
    m_treeView = new QTreeView();
    m_treeViewModel = new ProcessTreeModel(this);

    proxyModel = new ProcessFilterModel(this);
    proxyModel->setSourceModel(m_treeViewModel);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(0);

    m_tableView = new QTableView();
    m_tableView->setWordWrap(false);
    m_tableViewModel = new ProcessTableModel(this);
    QHeaderView *verticalHeader = m_tableView->verticalHeader();
    verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader->setDefaultSectionSize(24);

    findByNameTextEdit = new QLineEdit();
    connect(findByNameTextEdit, &QLineEdit::textChanged, [=](const QString &)
    {
        proxyModel->setSearchText(findByNameTextEdit->text());

    });
    findByNameTextEdit->setMinimumHeight(30);
    findByNameTextEdit->setMaximumHeight(30);
    findByNameTextEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_contextMenu = new QMenu(m_treeView);
    auto killProcessAction = new QAction(QCoreApplication::translate("MainWindow", "Kill process", nullptr), m_contextMenu);
    auto propertiesAction = new QAction(QCoreApplication::translate("MainWindow", "Properties...", nullptr), m_contextMenu);
    connect(killProcessAction, &QAction::triggered, this, &MainWindow::killProcessMenuActionClicked);
    connect(propertiesAction, &QAction::triggered, this, &MainWindow::propertiesMenuActionClicked);

    m_contextMenu->addAction(killProcessAction);
    m_contextMenu->addAction(propertiesAction);

    //(static_cast<QWidget*>(treeView->header()))->hide();
    m_treeView->setModel(proxyModel);
    m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);

     m_tableView->setModel(m_tableViewModel);
    auto header = m_treeView->header();
    header->resizeSection(0, 100);
    header->resizeSection(1, 100);
    header->resizeSection(2, 100);
    header->setSectionResizeMode(0, QHeaderView::Stretch);
    m_treeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    layout->addWidget(findByNameTextEdit);
    layout->addWidget(m_treeView);
    layout->addWidget(m_tableView);
    m_treeView->setVisible(m_displayAsTree);
    m_tableView->setVisible(!m_displayAsTree);
    layout->addWidget(pushButton);
    setCentralWidget(centralwidget);
    menubar = new QMenuBar(this);
    menubar->setObjectName("menubar");
    menubar->setGeometry(QRect(0, 0, 800, 22));
    setMenuBar(menubar);
    statusbar = new QStatusBar(this);
    statusbar->setObjectName("statusbar");
    setStatusBar(statusbar);

    retranslateUi();

    QMetaObject::connectSlotsByName(this);

    _processInfoTimer = new QTimer;
    _processInfoTimer->setSingleShot(false);
    connect(m_treeView, &QTreeView::customContextMenuRequested, this, &MainWindow::onProcessTreeContextMenu);
    connect(_processInfoTimer,&QTimer::timeout,this,[this]()
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

    _processInfoTimer->start(1000);
} // setupUi

void MainWindow::propertiesMenuActionClicked()
{
    auto indexes = m_treeView->selectionModel()->selectedIndexes();
    for (const auto& proxyIndex: indexes)
    {
        auto index = proxyModel->mapToSource(proxyIndex);
        if (index.column() == 0)
        {
            auto item = m_treeViewModel->getItemByIndex(index);
            ProcessProperties model;
            model.processId = item->getId();
            model.processName = item->getName();

            ProcessPropertiesDialog* otherWidget = new ProcessPropertiesDialog(model, this);
            QObject::connect(this, &QMainWindow::destroyed, otherWidget, &QWidget::deleteLater);
            otherWidget->show();
        }
    }
}
void MainWindow::killProcessMenuActionClicked()
{
    auto indexes = m_treeView->selectionModel()->selectedIndexes();
    for (const auto& proxyIndex: indexes)
    {
        auto index = proxyModel->mapToSource(proxyIndex);
        if (index.column() == 0)
        {
            auto item = m_treeViewModel->getItemByIndex(index);
            qDebug() << item->getName() << "Killing process" << item->getId() << item->getName();

            _processService->kill(item->getId());
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

void MainWindow::retranslateUi()
{

} // retranslateUi

MainWindow::~MainWindow()
{

}

void MainWindow::OnProcessInfoReceived(QFutureWatcher<std::map<unsigned int, ProcessInfo>>* processesStateWatcher)
{
    auto data = processesStateWatcher->result();

    if (m_displayAsTree)
    {
        m_treeViewModel->load(data);
    }
    else
    {
        m_tableViewModel->load(data);
    }
}

