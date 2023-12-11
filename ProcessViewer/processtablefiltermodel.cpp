#include "processtablefiltermodel.h"

ProcessTableFilterModel::ProcessTableFilterModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    setRecursiveFilteringEnabled(true);
}

bool ProcessTableFilterModel::lessThan ( const QModelIndex & left, const QModelIndex & right ) const
{
    switch (static_cast<ProcessTableModel::Properties>(left.column()))
    {
    case ProcessTableModel::Properties::PID:
    {
        auto model = static_cast<ProcessTableModel*>(sourceModel());
        auto l = model->getChildAtRow(left.row());
        auto r = model->getChildAtRow(right.row());
        return l->getId() < r->getId();
    }
    case ProcessTableModel::Properties::PrivateBytes:
    {
        auto model = static_cast<ProcessTableModel*>(sourceModel());
        auto l = model->getChildAtRow(left.row());
        auto r = model->getChildAtRow(right.row());
        return l->getPageFileUsage() < r->getPageFileUsage();
    }
    case ProcessTableModel::Properties::WorkingSet:
    {
        auto model = static_cast<ProcessTableModel*>(sourceModel());
        auto l = model->getChildAtRow(left.row());
        auto r = model->getChildAtRow(right.row());
        return l->getWorkingSetSize() < r->getWorkingSetSize();
    }
    case ProcessTableModel::Properties::CpuUsage:
    {
        auto model = static_cast<ProcessTableModel*>(sourceModel());
        auto l = model->getChildAtRow(left.row());
        auto r = model->getChildAtRow(right.row());
        return l->getCpuUsage() < r->getCpuUsage();
    }
    default:
        return QSortFilterProxyModel::lessThan(left, right);
    }
}

bool ProcessTableFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const
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

