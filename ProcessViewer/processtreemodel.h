#ifndef PROCESSTREEMODEL_H
#define PROCESSTREEMODEL_H

#include <QStandardItemModel>

#include <process.h>
#include <thread>
#include <QApplication>
#include "processinfo.h"
class ProcessTreeItem;

class ProcessTreeModel : public QAbstractItemModel
{
    // Q_OBJECT

    std::unordered_map<unsigned int, ProcessTreeItem*> itemsTree;
    unsigned int m_processorCount = std::thread::hardware_concurrency();
public:

    enum class Properties
    {
        ProcessName = 0,
        PID,
        PrivateBytes,
        WorkingSet,
        ExecutablePath,
        //        Frequency = 5,
        //        Percent = 6,
        CpuUsage,
        END
    };

    std::map<ProcessTreeModel::Properties, QString> PropertiesNames
        {
         {ProcessTreeModel::Properties::ProcessName, QCoreApplication::translate("ProcessTreeModel", "ProcessName", nullptr)},
         {ProcessTreeModel::Properties::PID, QCoreApplication::translate("ProcessTreeModel", "PID", nullptr)},
         {ProcessTreeModel::Properties::PrivateBytes, QCoreApplication::translate("ProcessTreeModel", "PrivateBytes", nullptr)},
         {ProcessTreeModel::Properties::WorkingSet, QCoreApplication::translate("ProcessTreeModel", "WorkingSet", nullptr)},
         {ProcessTreeModel::Properties::ExecutablePath, QCoreApplication::translate("ProcessTreeModel", "ExecutablePath", nullptr)},
         {ProcessTreeModel::Properties::CpuUsage, QCoreApplication::translate("ProcessTreeModel", "CpuUsage", nullptr)},
         };

    ProcessTreeModel(QObject *parent = 0);
    mutable std::unordered_map<size_t, QPersistentModelIndex> _persistentIndices;
    void load(std::map<unsigned int, ProcessInfo> &data);
    ~ProcessTreeModel();
    std::unordered_map<size_t, QPersistentModelIndex> getPersistentIndices()
    {
        return _persistentIndices;
    }
    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    ProcessTreeItem* getItemByIndex(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    void addItem(ProcessTreeItem* item, const QModelIndex& parentIndex);
    void removeItem(SIZE_T processId);
    bool updateRow(size_t id);

private:
    void setupModelData(const QStringList &lines, ProcessTreeItem *parent);
    ProcessTreeItem *m_rootItem;
};

#endif // PROCESSTREEMODEL_H
