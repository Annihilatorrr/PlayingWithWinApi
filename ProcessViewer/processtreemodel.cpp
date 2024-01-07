#include <map>
#include <QLocale>
#include "processtreemodel.h"
#include "processtreeitem.h"
#include "processinfo.h"

ProcessTreeModel::ProcessTreeModel(QObject *parent): QAbstractItemModel(parent)
{
    QList<QVariant> rootData;
    for(const auto& [key, propName]:PropertiesNames)
    {
        rootData << propName;
    }
    m_rootItem = new ProcessTreeItem(rootData);
}

void ProcessTreeModel::load(std::map<unsigned int, ProcessInfo> &processInfoRecords)
{
    //Create tree of processes. Item are being added or updated if already exist. Parents are not set here.
    for (auto& [processId, pi] : processInfoRecords)
    {
        auto existintgTreeItemIt = itemsTree.find(processId);
        if (existintgTreeItemIt != itemsTree.end())
        {
            auto& existingItem = *existintgTreeItemIt->second;
            existingItem.setName(QString::fromStdWString(pi.name));
            existingItem.setWorkingSetSize(pi.extendedInfo.memoryInfo.WorkingSetSize);
            existingItem.setPageFileUsage(pi.extendedInfo.memoryInfo.PageFileUsage);

            auto time = pi.perfData.frequency100Ns - existingItem.getFrequency();
            if (time != 0)
            {
                auto cpu = pi.perfData.percentProcessorTime - existingItem.getPercentage();
                double ratioPerCpuInPercents{double(cpu) / time/m_processorCount*100};
                double cpuUsage{std::roundf( ratioPerCpuInPercents*100)/100.0};

                if (existingItem.getId() != 0)
                {
                    itemsTree[0]->setCpuUsage(itemsTree[0]->getCpuUsage() - cpuUsage);
                }

                existingItem.setCpuUsage(cpuUsage);
            }
            existingItem.setPercentage(pi.perfData.percentProcessorTime);
            existingItem.setFrequency(pi.perfData.frequency100Ns);
        }
        else
        {
            auto newItem = new ProcessTreeItem(pi.id, QString::fromStdWString(pi.name), pi.extendedInfo.memoryInfo.WorkingSetSize, pi.extendedInfo.memoryInfo.PageFileUsage);
            newItem->setDescription(QString::fromStdWString(pi.description));

           if (!pi.executablePath.empty())
           {
               SHFILEINFO shfi;
               SHGetFileInfo(pi.executablePath.c_str(), FILE_ATTRIBUTE_NORMAL, &shfi,
                             sizeof(shfi), SHGFI_ICON | SHGFI_USEFILEATTRIBUTES| SHGFI_LARGEICON| SHGFI_SMALLICON);
               if (shfi.hIcon != nullptr)
               {
                   qDebug() << "Setting icon for" << newItem->getId() << newItem->getName();
                   QImage image(QImage::fromHICON(shfi.hIcon));
                   QPixmap processIcon{QPixmap::fromImage(image)};
                   auto scaledProcessIcon{processIcon.scaled(16, 16, Qt::KeepAspectRatio)};
                   newItem->setIcon(image);
               }
           }
            newItem->setExecutablePath(QString::fromStdWString(pi.executablePath));
            newItem->setPercentage(pi.perfData.percentProcessorTime);
            newItem->setFrequency(pi.perfData.frequency100Ns);
            itemsTree[pi.id] = newItem;
        }
    }
    //emit layoutAboutToBeChanged();
    // set parents for all existing items
    for (auto& [processId, pi] : processInfoRecords)
    {
        auto& childItem{itemsTree[processId]};
        if (processId != 0)
        {
            if (auto foundParent = itemsTree.find(pi.parentProcessId); foundParent != itemsTree.end())
            {
                bool containsChild = foundParent->second->contains(processId);
                if (!containsChild)
                {
                    auto persistentParentIndexIt = _persistentIndices.find(pi.parentProcessId);
                    if (persistentParentIndexIt != _persistentIndices.end())
                    {
                        qDebug() << "Persistent index exists for parent" << foundParent->second->getName() << "of" << itemsTree[processId]->getName();
                        addItem(childItem, persistentParentIndexIt->second);
                    }
                    else
                    {
                        foundParent->second->addChild(childItem);
                    }
                }
            }
            else // if parent is non-existent process
            {
                ProcessTreeItem* existintgChild = m_rootItem->getChildById(processId);
                if (!existintgChild)
                {
                    // append it as highest level process
                    addItem(childItem, QModelIndex());
                }
            }
        }
        else
        {
            bool containsChild = m_rootItem->contains(processId);
            if (!containsChild)
            {
                //m_rootItem->addChild(childItem);
                addItem(childItem, QModelIndex());
            }
        }
        if (itemsTree[processId]->isDirty())
        {
            updateRow(processId);
            //qDebug() << "Updated" << processId;
            itemsTree[processId]->resetDirty();
        }
    }
    //emit layoutChanged();
    //updateRow(processId);
    auto itemsTreeIt = itemsTree.begin();
    std::unordered_map<SIZE_T, ProcessTreeItem*> outdatedItemsIds;
    while(itemsTreeIt != itemsTree.end())
    {
        if (!processInfoRecords.contains(itemsTreeIt->second->getId()))
        {
            //qDebug() << tr("Proccess")  << itemsTreeIt->second->getId() << itemsTreeIt->second->getName() << "needs to be removed";
            //qDebug() << "(";
            for(auto&& childId: itemsTreeIt->second->getIdsWithChildren())
            {
                outdatedItemsIds.insert({childId, itemsTree.at(childId)});
            }
            //qDebug() << ")";

        }
        ++itemsTreeIt;
    }

    qDebug() << "---- Entire list of processes to remove:";
    for (auto& [id, processItem]: outdatedItemsIds)
    {
        //qDebug() << "\t Erasing of" << id << processItem->getName();
        removeItem(id);
        //qDebug() << "Record removed";
        itemsTree.erase(id);
        _persistentIndices.erase(id);
        processItem->setReadyToDelete();
    }
    for (const auto& [id, processItem]: outdatedItemsIds)
    {
        if (processItem->isReadyToDelete())
        {
            delete processItem;
        }
    }
}

ProcessTreeModel::~ProcessTreeModel()
{
    delete m_rootItem;
}

int ProcessTreeModel::columnCount(const QModelIndex &parent) const
{
    return static_cast<int>(ProcessTreeModel::Properties::END);
}


QVariant ProcessTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    switch (role)
    {
    case  Qt::ToolTipRole:
    {
        ProcessTreeItem *item = getItemByIndex(index);
        QLocale aEnglish;
        switch((Properties)index.column())
        {
        case Properties::ProcessName:
            return item->getName();
        case Properties::ExecutablePath:
            return item->getExecutablePath();
        default:
            return QVariant();
        }
    }
    case  Qt::DecorationRole:
    {
        if (index.column() != 0)
        {
            return QVariant();
        }
        ProcessTreeItem *item = getItemByIndex(index);
        return item->getIcon();
    }
    default:
    {
        return QVariant();
    }
    case Qt::DisplayRole:
    {
        ProcessTreeItem *item = getItemByIndex(index);

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
            return QString("%L1 K").arg(item->getWorkingSetSize());
        }
        case Properties::ExecutablePath:
            return item->getExecutablePath();
        case Properties::CpuUsage:
            return QString("%L1 %").arg(item->getCpuUsage());
        default:
            return {};
        }
    }
    }
}
Qt::ItemFlags ProcessTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant ProcessTreeModel::headerData(int section, Qt::Orientation orientation,
                                      int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return PropertiesNames.at((ProcessTreeModel::Properties)section);

    return QVariant();
}

bool ProcessTreeModel::setData(const QModelIndex &index,
                               const QVariant &value, int role)
{
    if (!index.isValid() || index.column() != 1)
        return false;
    if (ProcessTreeItem *item = getItemByIndex(index))
    {
        item->setProperty([&](ProcessTreeItem& treeItem){treeItem.setName(value.toString());});
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

void ProcessTreeModel::removeItem(SIZE_T processId)
{
    QModelIndex parentIndex;

    ProcessTreeItem* item = itemsTree.at(processId);
    if (item == nullptr)
    {
        return;
    }
    auto parentId = item->getParent()->getId();
    auto parentIndexIt = _persistentIndices.find(parentId);

    // if direct parent is visible (or has been made visible so far), change internal structure
    // and inform the view about it
    if (parentIndexIt != _persistentIndices.end())
    {
        int firstRow = item->getRow();
        int lastRow = item->getRow();
        parentIndex = parentIndexIt->second;
        auto parentItem{getItemByIndex(parentIndex)};
        beginRemoveRows(parentIndex, firstRow, lastRow);
        parentItem->removeChild(firstRow);
        endRemoveRows();
        return;
    }

    // if direct parent is not visible (or hasn't been made visible so far) or
    // direct parent is rootItem (it doesn't have persistent index created)
    int row = item->getRow();
    auto parent = item->getParent();
    if (parent == m_rootItem)
    {
        beginRemoveRows(QModelIndex(), row, row);
        parent->removeChild(row);
        endRemoveRows();
    }
    else // direct parent is not visible and not a tootItem
    // just change internal structure without informing the view
    {
        parent->removeChild(row);
    }
}

void ProcessTreeModel::addItem(ProcessTreeItem* item, const QModelIndex& parentIndex)
{
    beginInsertRows(parentIndex, rowCount(parentIndex), rowCount(parentIndex));
    getItemByIndex(parentIndex)->addChild(item);
    endInsertRows();
}
bool ProcessTreeModel::updateRow(SIZE_T processId)
{
    auto indexIt = _persistentIndices.find(processId);
    if(indexIt == _persistentIndices.end() || !indexIt->second.isValid())
    {
        return false;
    }

    auto idx = indexIt->second;

    QModelIndex fromIndex = index(idx.row(), 0, idx.parent());
    QModelIndex toIndex = index(idx.row(), static_cast<int>(Properties::END) - 1, idx.parent());

    emit dataChanged(fromIndex, toIndex);
    return true;
}

QModelIndex ProcessTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    ProcessTreeItem *parentItem = getItemByIndex(parent);

    //qDebug() << "Creating index for parent" << parentItem->getId() << "row = " << row << " column = " << column << "at parent = " << parentItem->getId();
    if (row < parentItem->getChildCount())
    {
        ProcessTreeItem *childItem = parentItem->getChildAt(row);
        if (childItem)
        {
            QModelIndex index = createIndex(row, column, childItem);
            auto _persistentIndexIt = _persistentIndices.find(childItem->getId());
            if (_persistentIndexIt == _persistentIndices.end())
            {
                QPersistentModelIndex persistentIndex(index);
                _persistentIndices.insert({childItem->getId(), persistentIndex});
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

QModelIndex ProcessTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return QModelIndex();
    }

    ProcessTreeItem *childItem = getItemByIndex(index);
    if (childItem == m_rootItem)
    {
        return QModelIndex();
    }
    ProcessTreeItem *parentItem = childItem->getParent();

    if (parentItem == nullptr || parentItem == m_rootItem)
    {
        return QModelIndex();
    }

    if (parentItem->getParent() == nullptr) return QModelIndex();
    return createIndex(parentItem->getRow(), 0, parentItem);
}

int ProcessTreeModel::rowCount(const QModelIndex &parent) const
{
    ProcessTreeItem *parentItem = getItemByIndex(parent);
    return parentItem->getChildCount();
}

ProcessTreeItem* ProcessTreeModel::getItemByIndex(const QModelIndex &index) const
{
    if (index.isValid())
    {
        auto item = static_cast<ProcessTreeItem*>(index.internalPointer());
        return item;
    }
    return m_rootItem;
}
