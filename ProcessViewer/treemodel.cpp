#include <map>
#include <QLocale>
#include "treemodel.h"
#include "treeitem.h"
#include "processinfo.h"

TreeModel::TreeModel(QObject *parent): QAbstractItemModel(parent)
{
    QList<QVariant> rootData;
    for(const auto& [key, propName]:PropertiesNames)
    {
        rootData << propName;
    }
    m_rootItem = new TreeItem(rootData);
}

void TreeModel::load(std::map<unsigned int, ProcessInfo> &processInfoRecords)
{
    //Create tree of processes. Item are being added or updated if already exist. Parents are not set here.
    for (auto& [processId, pi] : processInfoRecords)
    {
        auto existintgTreeItemIt = itemsTree.find(processId);
        if (existintgTreeItemIt == itemsTree.end())
        {
            itemsTree[pi.id] = new TreeItem(pi.id, QString::fromStdWString(pi.name), pi.extendedInfo.memoryInfo.WorkingSetSize, pi.extendedInfo.memoryInfo.PageFileUsage);
            itemsTree[pi.id]->setDescription(QString::fromStdWString(pi.description));
            itemsTree[pi.id]->setExecutablePath(QString::fromStdWString(pi.executablePath));
            itemsTree[pi.id]->setPercentage(pi.perfData.percentProcessorTime);
            itemsTree[pi.id]->setFrequency(pi.perfData.frequency100Ns);
        }
        else
        {
            existintgTreeItemIt->second->setName(QString::fromStdWString(pi.name));
            existintgTreeItemIt->second->setWorkingSetSize(pi.extendedInfo.memoryInfo.WorkingSetSize);
            existintgTreeItemIt->second->setPageFileUsage(pi.extendedInfo.memoryInfo.PageFileUsage);

            auto time = pi.perfData.frequency100Ns - existintgTreeItemIt->second->getFrequency();
            if (time != 0)
            {
                auto cpu = pi.perfData.percentProcessorTime - existintgTreeItemIt->second->getPercentage();
                double ratioPerCpuInPercents{double(cpu) / time/m_processorCount*100};
                double cpuUsage{std::roundf( ratioPerCpuInPercents*100)/100.0};
                existintgTreeItemIt->second->setCpuUsage(cpuUsage);
            }
            existintgTreeItemIt->second->setPercentage(pi.perfData.percentProcessorTime);
            existintgTreeItemIt->second->setFrequency(pi.perfData.frequency100Ns);
        }
    }

    // set parents for all existing items
    for (auto& [processId, pi] : processInfoRecords)
    {
        if (processId != 0)
        {
            if (auto foundParent = itemsTree.find(pi.parentProcessId); foundParent != itemsTree.end())
            {
                TreeItem* existintgChild = foundParent->second->getChildById(processId);
                if (!existintgChild)
                {
                    foundParent->second->addChild(itemsTree[processId]);
                    auto persistentParentIndexIt = _persistentIndices.find(pi.parentProcessId);
                    if (persistentParentIndexIt != _persistentIndices.end())
                    {
                        addItem(itemsTree[processId], persistentParentIndexIt->second);
                    }
                }
                updateRow(processId);
            }
            else // non is non existing process
            {
                //qDebug() << "Parent process not found for process" << processId;
                TreeItem* existintgChild = m_rootItem->getChildById(processId);
                if (!existintgChild)
                {
                    m_rootItem->addChild(itemsTree[processId]);
                    addItem(itemsTree[processId], QModelIndex());
                }
                updateRow(processId);
            }
        }
        else
        {
            TreeItem* existintgChild = m_rootItem->getChildById(processId);
            if (!existintgChild)
            {
                m_rootItem->addChild(itemsTree[processId]);
                //addItem(itemsTree[processId], QModelIndex());
            }
            updateRow(processId);
        }
    }

    auto itemsTreeIt = itemsTree.begin();
    std::map<SIZE_T, TreeItem*> outdatedItemsIds;
    while(itemsTreeIt != itemsTree.end())
    {
        if (!processInfoRecords.contains(itemsTreeIt->second->getId()))
        {
            qDebug() << tr("Proccess")  << itemsTreeIt->second->getId() << itemsTreeIt->second->getName() << "needs to be removed";
            qDebug() << "(";
            for(const auto& childId: itemsTreeIt->second->getIdsWithChildren())
            {
                try
                {
                    outdatedItemsIds.insert({childId, itemsTree.at(childId)});
                }
                catch (std::out_of_range& exc)
                {
                    qDebug() << "Process" << childId << "is not present in items tree";
                }
            }
            qDebug() << ")";

        }
        ++itemsTreeIt;
    }

    qDebug() << "---- Entire list of processes to remove:";
    for (auto& [id, processItem]: outdatedItemsIds)
    {
        qDebug() << "\t Erasing of" << id << processItem->getName();
        auto recordRemoved = removeItem(id);
        qDebug() << "Record removed" << recordRemoved;
        if (recordRemoved)
        {
            itemsTree.erase(id);
            processItem->setReadyToDelete();
        }
        //processItem->clearChildren();
    }
    for (const auto& [id, processItem]: outdatedItemsIds)
    {
        if (processItem->isReadyToDelete())
        {
            delete processItem;
        }
    }
}

TreeModel::~TreeModel()
{
    delete m_rootItem;
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    return static_cast<int>(TreeModel::Properties::END);
}


QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    if (role == Qt::ToolTipRole)
    {
        TreeItem *item = getItemByIndex(index);

        QLocale aEnglish;
        switch((Properties)index.column())
        {
        case Properties::ProcessName:
            return item->getName();
        case Properties::ExecutablePath:
            return item->getExecutablePath();
        default:
            return {};
        }
    }
    if ((role != Qt::DisplayRole))
    {
        return QVariant();
    }
    TreeItem *item = getItemByIndex(index);

    QLocale aEnglish;
    switch((Properties)index.column())
    {
    case Properties::ProcessName:
        return item->getName();
    case Properties::PID:
        return item->getId();
    case Properties::PrivateBytes:
    {
        return QString("%L1 K").arg(item->getPageFileUsage());
    }
    case Properties::WorkingSet:
    {
        return QString("%L1 K").arg(item->getMorkingSetSize());
    }
    case Properties::ExecutablePath:
        return item->getExecutablePath();
    case Properties::CpuUsage:
        return QString("%L1 %").arg(item->getCpuUsage());
    default:
        return {};
    }
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return PropertiesNames.at((TreeModel::Properties)section);

    return QVariant();
}

bool TreeModel::setData(const QModelIndex &index,
                        const QVariant &value, int role)
{
    if (!index.isValid() || index.column() != 1)
        return false;
    if (TreeItem *item = getItemByIndex(index))
    {
        item->setProperty([&](TreeItem& treeItem){treeItem.setName(value.toString());});
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

bool TreeModel::removeItem(SIZE_T processId)
{
    QModelIndex parentIndex;

    TreeItem* item = itemsTree.at(processId);
    auto parentId = item->getParent()->getId();
    auto parentIndexIt = _persistentIndices.find(parentId);

    // if direct parent is visible (or has been made visible so far), change internal structure
    // and inform the view about it
    if (parentIndexIt != _persistentIndices.end())
    {
        int row = item->getRow();
        parentIndex = parentIndexIt->second;
        qDebug() << "Removing by parent, row" << row;
        beginRemoveRows(parentIndex, row, row);
        getItemByIndex(parentIndex)->removeChild(row);
        endRemoveRows();
        _persistentIndices.erase(processId);
        return true;
    }

    // if direct parent is not visible (or hasn't been made visible so far) or rootItem (it doesn't have persistent index created), just change internal structure
    // without informing the view
    int row = item->getRow();
    auto parent = item->getParent();
    if (parent == m_rootItem)
    {
        beginRemoveRows(QModelIndex(), row, row);
        parent->removeChild(row);
        endRemoveRows();
    }
    else
    {
        parent->removeChild(row);
    }
    _persistentIndices.erase(processId);
    return true;
}

void TreeModel::addItem(TreeItem* item, const QModelIndex& parentIndex)
{
    beginInsertRows(parentIndex, rowCount(parentIndex), rowCount(parentIndex));
    endInsertRows();
}
bool TreeModel::updateRow(SIZE_T processId)
{
    auto indexIt = _persistentIndices.find(processId);
    if(indexIt == _persistentIndices.end() || !indexIt->second.isValid())
    {
        return false;
    }

    auto idx = indexIt->second;

    if (!idx.isValid() || idx.column() < 0)
    {
        return false;
    }
    if (TreeItem *item = getItemByIndex(idx))
    {
        QModelIndex fromIndex = index(idx.row(), 0, idx.parent());
        QModelIndex toIndex = index(idx.row(), static_cast<int>(Properties::END) - 1, idx.parent());

        emit dataChanged(fromIndex, toIndex);
        //qDebug() <<"Data changed emitted";
        return true;
    }
    return false;
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
    TreeItem *parentItem = getItemByIndex(parent);

    //qDebug() << "Creating index for row = " << row << " column = " << column << "at parent = " << parentItem->getId();
    if (row < parentItem->getChildCount())
    {
        TreeItem *childItem = parentItem->getChildAt(row);
        //qDebug() << "Creating index for row = " << row << " column = " << column;
        if (childItem)
        {
            QModelIndex index = createIndex(row, column, childItem);
            auto _persistentIndexIt = _persistentIndices.find(childItem->getId());
            if (_persistentIndexIt == _persistentIndices.end())
            {
                QPersistentModelIndex persistentIndex(index);
                _persistentIndices.insert({childItem->getId(), persistentIndex});
                qDebug() << "Creating persistent index for ID = " << childItem->getId() << childItem->getName() ;
            }
            return index;
        }
        else
        {
            return QModelIndex();
        }
    }
    return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return QModelIndex();
    }

    TreeItem *childItem = getItemByIndex(index);
    if (childItem == m_rootItem)
    {
        return QModelIndex();
    }
    TreeItem *parentItem = childItem->getParent();

    if (parentItem == nullptr || parentItem == m_rootItem)
    {
        return QModelIndex();
    }

    if (parentItem->getParent() == nullptr) return QModelIndex();
    return createIndex(parentItem->getRow(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem = getItemByIndex(parent);
    return parentItem->getChildCount();
}

TreeItem* TreeModel::getItemByIndex(const QModelIndex &index) const
{
    return (index.isValid()) ? static_cast<TreeItem*>(index.internalPointer()) : m_rootItem;
}
