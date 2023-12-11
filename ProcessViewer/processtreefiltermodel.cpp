#include "processtreefiltermodel.h"
#include "processtreemodel.h"
#include "processtreeitem.h"
ProcessTreeFilterModel::ProcessTreeFilterModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    setRecursiveFilteringEnabled(true);
}

bool ProcessTreeFilterModel::lessThan( const QModelIndex & left, const QModelIndex & right ) const
{
    switch (static_cast<ProcessTreeModel::Properties>(left.column()))
    {
    case ProcessTreeModel::Properties::PID:
    {
        auto model = static_cast<ProcessTreeModel*>(sourceModel());
        auto children = model->children();
        auto l = model->getItemByIndex(left);
        auto r = model->getItemByIndex(right);
        return l->getId() < r->getId();
    }
    case ProcessTreeModel::Properties::PrivateBytes:
    {
        auto model = static_cast<ProcessTreeModel*>(sourceModel());
        auto children = model->children();
        auto l = model->getItemByIndex(left);
        auto r = model->getItemByIndex(right);
        return l->getPageFileUsage() < r->getPageFileUsage();
    }
    case ProcessTreeModel::Properties::WorkingSet:
    {
        auto model = static_cast<ProcessTreeModel*>(sourceModel());
        auto children = model->children();
        auto l = model->getItemByIndex(left);
        auto r = model->getItemByIndex(right);
        return l->getWorkingSetSize() < r->getWorkingSetSize();
    }
    case ProcessTreeModel::Properties::CpuUsage:
    {
        auto model = static_cast<ProcessTreeModel*>(sourceModel());
        auto children = model->children();
        auto l = model->getItemByIndex(left);
        auto r = model->getItemByIndex(right);
        return l->getCpuUsage() < r->getCpuUsage();
    }
    default:
        return QSortFilterProxyModel::lessThan(left, right);
    }
}

bool ProcessTreeFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const
{
    // check the current item
    bool result{_searchStrings.empty()};
    for(const auto& [key, value]: _searchStrings)
    {
        QModelIndex currntIndex = sourceModel()->index(sourceRow, static_cast<int>(key), sourceParent);
        auto model = sourceModel();
        auto item = model->data(currntIndex, Qt::DisplayRole).value<QString>();
        auto contains = item.contains(value, Qt::CaseInsensitive);
        result = result || contains;
    }
    return result;
}

