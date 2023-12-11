#ifndef PROCESSTABLEMODEL_H
#define PROCESSTABLEMODEL_H

#include <QStandardItemModel>
#include "processinfo.h"
#include "processtreeitem.h"
class ProcessTableModel : public QAbstractTableModel
{
    Q_OBJECT
    ProcessTreeItem *m_rootItem;
    std::map<unsigned int, ProcessTreeItem*> itemsTree;
    std::vector<ProcessTreeItem*> children;
    unsigned int m_processorCount = std::thread::hardware_concurrency();
    mutable std::map<size_t, QPersistentModelIndex> _persistentIndices;
public:

    ProcessTreeItem* getItemByIndex(const QModelIndex &index) const;
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

    std::map<ProcessTableModel::Properties, QString> PropertiesNames
        {
         {ProcessTableModel::Properties::ProcessName, "ProcessName"},
         {ProcessTableModel::Properties::PID, "PID"},
         {ProcessTableModel::Properties::PrivateBytes, "Private Bytes"},
         {ProcessTableModel::Properties::WorkingSet, "Working Set"},
         {ProcessTableModel::Properties::ExecutablePath, "ExecutablePath"},
         {ProcessTableModel::Properties::CpuUsage, "Cpu Usage"},
         };

    ProcessTableModel(QObject * parent = 0);
    bool updateRow(SIZE_T processId);
    void addItem(ProcessTreeItem* item, const QModelIndex& parentIndex);
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        return parent.isValid() ? 0 :itemsTree.size();
    }
    bool insertRows(int row, int count, const QModelIndex &) override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override{return true;}
    void load(std::map<unsigned int, ProcessInfo> &data);
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    void removeItem(SIZE_T processId);
    ProcessTreeItem* getChildAtRow(int row)
    {
        return children.at(row);
    }
};

#endif // PROCESSTABLEMODEL_H
