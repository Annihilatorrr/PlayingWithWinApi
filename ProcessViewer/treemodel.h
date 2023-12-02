#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QStandardItemModel>

#include <process.h>
#include <vector>
#include "processinfo.h"
class TreeItem;

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

    std::map<unsigned int, TreeItem*> itemsTree;
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

    std::map<TreeModel::Properties, QString> PropertiesNames
        {
         {TreeModel::Properties::ProcessName, "ProcessName"},
         {TreeModel::Properties::PID, "PID"},
         {TreeModel::Properties::PrivateBytes, "Private Bytes"},
         {TreeModel::Properties::WorkingSet, "Working Set"},
         {TreeModel::Properties::ExecutablePath, "ExecutablePath"},
         {TreeModel::Properties::CpuUsage, "Cpu Usage"},
         };

    TreeModel(QObject *parent = 0);
    mutable std::map<size_t, QPersistentModelIndex> _persistentIndices;
    void load(std::map<unsigned int, ProcessInfo> &data);
    ~TreeModel();
    std::map<size_t, QPersistentModelIndex> getPersistentIndices()
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
    TreeItem* getItemByIndex(const QModelIndex &index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    void addItem(TreeItem* item, const QModelIndex& parentIndex);
    bool removeItem(SIZE_T processId);
    bool updateRow(size_t id);

private:
    void setupModelData(const QStringList &lines, TreeItem *parent);
    TreeItem *m_rootItem;
};

#endif // TREEMODEL_H