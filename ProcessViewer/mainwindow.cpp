#include "mainwindow.h"
#include "treeitem.h"
#include "processserviceexception.h"
#include <QLayout>
#include <QtConcurrent/QtConcurrent>
#include <QHeaderView>
#include <process.h>
#include <ranges>
MainWindow::MainWindow(const IProcessService& processService, QWidget *parent)
    : QMainWindow(parent), _processService(processService)
{
    setupUi();
}

std::map<unsigned int, ProcessInfo> MainWindow::getProcesses()
{
    std::map<unsigned int, ProcessInfo> processes = _processService.getProcessTreeByCom();
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
    if (objectName().isEmpty())
        setObjectName("MainWindow");
    resize(800, 600);
    centralwidget = new QWidget(this);
    centralwidget->setObjectName("centralwidget");
    QVBoxLayout* layout = new QVBoxLayout(centralwidget);
    pushButton = new QPushButton();
    pushButton->setObjectName("pushButton");



    model = new TreeModel(this);
    treeView = new QTreeView();

    //(static_cast<QWidget*>(treeView->header()))->hide();
    treeView->setModel(model);
    treeView->header()->resizeSection(0, 100);
    treeView->header()->resizeSection(1, 100);
    treeView->header()->resizeSection(2, 100);
    layout->addWidget(treeView);
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
    connect(_processInfoTimer,&QTimer::timeout,this,[this]()
            {
                auto processesStateWatcher = new QFutureWatcher<std::map<unsigned int, ProcessInfo>>();
                currentProcessesResultFuture = QtConcurrent::run(&MainWindow::getProcesses, this);
                //qDebug() << "processesStateWatcher connected";
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

void MainWindow::retranslateUi()
{
    setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
    pushButton->setText(QCoreApplication::translate("MainWindow", "PushButton", nullptr));
} // retranslateUi

MainWindow::~MainWindow()
{

}

void MainWindow::OnProcessInfoReceived(QFutureWatcher<std::map<unsigned int, ProcessInfo>>* processesStateWatcher)
{
    auto data = processesStateWatcher->result();
    model->load(data);
    auto header = treeView->header();
    header->setSectionResizeMode(0, QHeaderView::Stretch);
}

