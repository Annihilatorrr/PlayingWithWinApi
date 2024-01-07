#include "processtablemodel.h"
#include <algorithm>
#include "processtreeitem.h"

ProcessTableModel::ProcessTableModel(QObject * parent):QAbstractTableModel(parent)
{
    QList<QVariant> rootData;
    for(const auto& [key, propName]:PropertiesNames)
    {
        rootData << propName;
    }
    m_rootItem = new ProcessTreeItem(rootData);
}

void ProcessTableModel::addItem(ProcessTreeItem* item, const QModelIndex& parentIndex)
{
    auto rc = rowCount(parentIndex);
    beginInsertRows(parentIndex, rc, rc);
    itemsTree[item->getId()] = item;
    children.push_back(item);
    endInsertRows();
}

QVariant ProcessTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }

    if (index.row() >= itemsTree.size())
    {
        return QVariant();
    }

    switch (role)
    {
    case  Qt::ToolTipRole:
    {
        const ProcessTreeItem *item= children[index.row()];
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
    case Qt::DisplayRole:
    {
        const ProcessTreeItem *item= children[index.row()];
        if (item != nullptr)
        {
            switch ((Properties)index.column())
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
                return QVariant();
            }
        }
    }
    case  Qt::DecorationRole:
    {
        if (index.column() != 0)
        {
            return QVariant();
        }
        const ProcessTreeItem *item = children[index.row()];
        return item->getIcon();
    }
    }
    return QVariant();
}

bool ProcessTableModel::updateRow(SIZE_T processId)
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
    if (ProcessTreeItem *item = itemsTree[processId])
    {
        QModelIndex fromIndex = index(idx.row(), 0, QModelIndex());
        QModelIndex toIndex = index(idx.row(), static_cast<int>(Properties::END) - 1, QModelIndex());

        emit dataChanged(fromIndex, toIndex);
        //qDebug() <<"Data changed emitted";
        return true;
    }
    return false;
}

// ProcessTreeItem* ProcessTableModel::getItemByIndex(const QModelIndex &index) const
// {
//     if (index.isValid())
//     {
//         return static_cast<ProcessTreeItem*>(index.internalPointer());
//     }
//     else
//     {
//         return m_rootItem;
//     }
// }
void ProcessTableModel::load(std::map<unsigned int, ProcessInfo> &processInfoRecords)
{
    //Create tree of processes. Item are being added or updated if already exist. Parents are not set here.
    for (auto& [processId, pi] : processInfoRecords)
    {
        auto existintgTreeItemIt = itemsTree.find(processId);
        if (existintgTreeItemIt == itemsTree.end())
        {
            auto newItem = new ProcessTreeItem(pi.id, QString::fromStdWString(pi.name), pi.extendedInfo.memoryInfo.WorkingSetSize, pi.extendedInfo.memoryInfo.PageFileUsage);
            newItem->setDescription(QString::fromStdWString(pi.description));
            newItem->setExecutablePath(QString::fromStdWString(pi.executablePath));
            newItem->setPercentage(pi.perfData.percentProcessorTime);
            newItem->setFrequency(pi.perfData.frequency100Ns);

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

            m_rootItem->addChild(newItem);
            addItem(newItem, QModelIndex());
        }
        else
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
            if (itemsTree[processId]->isDirty())
            {
                updateRow(processId);
                itemsTree[processId]->resetDirty();
            }
        }
    }

    auto itemsTreeIt = itemsTree.begin();
    std::map<SIZE_T, ProcessTreeItem*> outdatedItemsIds;
    while(itemsTreeIt != itemsTree.end())
    {
        if (!processInfoRecords.contains(itemsTreeIt->second->getId()))
        {
            qDebug() << tr("Proccess")  << itemsTreeIt->second->getId() << itemsTreeIt->second->getName() << "needs to be removed";
            for(auto&& childId: itemsTreeIt->second->getIdsWithChildren())
            {
                outdatedItemsIds.insert({childId, itemsTree.at(childId)});
            }
        }
        ++itemsTreeIt;
    }

    qDebug() << "---- Entire list of processes to remove:";
    for (auto& [id, processItem]: outdatedItemsIds)
    {
        qDebug() << "\t Erasing of" << id << processItem->getName();

        std::vector<ProcessTreeItem*>::iterator position = std::find(children.begin(), children.end(), processItem);
        if (position != children.end()) // == myVector.end() means the element was not found
        {
            children.erase(position);
        }
        removeItem(id);
        qDebug() << "Record removed";
        _persistentIndices.erase(id);
        itemsTree.erase(id);
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

void ProcessTableModel::removeItem(SIZE_T processId)
{
    ProcessTreeItem* item = itemsTree.at(processId);

    // if direct parent is not visible (or hasn't been made visible so far) or
    // direct parent is rootItem (it doesn't have persistent index created)
    int row = item->getRow();

    beginRemoveRows(QModelIndex(), row, row);
    m_rootItem->removeChild(row);
    endRemoveRows();

}

QModelIndex ProcessTableModel::index(int row, int column, const QModelIndex &parent) const
{
    if (hasIndex(row, column, parent))
    {
        const ProcessTreeItem *parentItem = getChildAtRow(parent.row());
        auto index = createIndex(row, column);
        ProcessTreeItem *childItem = parentItem->getChildAt(row);
        auto _persistentIndexIt = _persistentIndices.find(childItem->getId());
        if (_persistentIndexIt == _persistentIndices.end())
        {
            QPersistentModelIndex persistentIndex(index);
            _persistentIndices.insert({childItem->getId(), persistentIndex});
            //qDebug() << "Creating persistent index for ID = " << childItem->getId() << childItem->getName() ;
        }
        return index;
    }
    return QModelIndex();
}

int ProcessTableModel::columnCount(const QModelIndex &parent) const
{
    return static_cast<int>(ProcessTableModel::Properties::END);
}


Qt::ItemFlags ProcessTableModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags theFlags = QAbstractTableModel::flags(index);
    if (index.isValid())
        theFlags |= Qt::ItemIsSelectable|Qt::ItemIsEditable|
                    Qt::ItemIsEnabled;
    return theFlags;
}

QVariant ProcessTableModel::headerData(int section, Qt::Orientation orientation,
                                       int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        return PropertiesNames.at((ProcessTableModel::Properties)section);
    }
    return QVariant();
}
