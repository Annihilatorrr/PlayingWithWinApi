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
                connect(processesStateWatcher, &QFutureWatcher<std::map<unsigned int, ProcessInfo>>::finished, this,
                        [=]
                        {
                            try
                            {
                                auto data = processesStateWatcher->result();
                                model->load(data);
                                auto header = treeView->header();
                                header->setSectionResizeMode(0, QHeaderView::Stretch);
                                //model->addItem(new TreeItem(500, QString::fromStdWString(std::format(L"{}_{:%F %T}", L"Pr1_", std::chrono::system_clock::now())), 123, 456), QModelIndex());
                                //treeView->setModel(model);
                                // model->addItem(new TreeItem(500, "500", 123, 456), QModelIndex());
                                //    auto isv = model->_persistentIndices[10].isValid();
                                //    for (const auto& pair: model->getPersistentIndices())
                                //    {
                                //        auto isValid = pair.isValid();
                                //        if (isValid)
                                //        {
                                //            treeView->setExpanded(pair, true);
                                //        }
                                //    }
                                //    treeView->setUpdatesEnabled(true);
                                disconnect(processesStateWatcher, &QFutureWatcher<std::map<unsigned int, ProcessInfo>>::finished, this, &MainWindow::FinishedFormat);
                                //qDebug() << "processesStateWatcher disconnected";
                            }
                            catch(ProcessServiceException& exc)
                            {
                                disconnect(processesStateWatcher, &QFutureWatcher<std::map<unsigned int, ProcessInfo>>::finished, this, &MainWindow::FinishedFormat);
                                //qDebug() << "exception thrown while receiving list of processes";
                                //qDebug() << "processesStateWatcher diconnected";
                            }
                        });
                connect(processesStateWatcher, &QFutureWatcher<std::map<unsigned int, ProcessInfo>>::finished, processesStateWatcher, &QFutureWatcher<std::map<unsigned int, ProcessInfo>>::deleteLater);
                processesStateWatcher->setFuture(currentProcessesResultFuture);
            });

    connect(pushButton, &QPushButton::clicked, [this](bool checked = false)
            {
                //qDebug() << "clicked";
                //model->addItem(new TreeItem(500, QString::fromStdWString(std::format(L"{}_{:%F %T}", L"Pr1_", std::chrono::system_clock::now())), 123, 456), QModelIndex());
                    //model->setName(1, QString("sdf"));
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

void MainWindow::FinishedFormat()
{
    try
    {
//        auto data = processesStateWatcher->result();
//        model->load(data);
//        auto header = treeView->header();
//        header->setSectionResizeMode(0, QHeaderView::Stretch);
        //model->addItem(new TreeItem(500, QString::fromStdWString(std::format(L"{}_{:%F %T}", L"Pr1_", std::chrono::system_clock::now())), 123, 456), QModelIndex());
        //treeView->setModel(model);
        // model->addItem(new TreeItem(500, "500", 123, 456), QModelIndex());
        //    auto isv = model->_persistentIndices[10].isValid();
        //    for (const auto& pair: model->getPersistentIndices())
        //    {
        //        auto isValid = pair.isValid();
        //        if (isValid)
        //        {
        //            treeView->setExpanded(pair, true);
        //        }
        //    }
        //    treeView->setUpdatesEnabled(true);
        //disconnect(processesStateWatcher, &QFutureWatcher<std::map<unsigned int, ProcessInfo>>::finished, this, &MainWindow::FinishedFormat);
        //qDebug() << "processesStateWatcher disconnected";
    }
    catch(ProcessServiceException& exc)
    {
        //disconnect(processesStateWatcher, &QFutureWatcher<std::map<unsigned int, ProcessInfo>>::finished, this, &MainWindow::FinishedFormat);
        //qDebug() << "exception thrown while receiving list of processes";
        //qDebug() << "processesStateWatcher diconnected";
    }
}

