#ifndef PROCESSTREEFILTERMODEL_H
#define PROCESSTREEFILTERMODEL_H

#include <QSortFilterProxyModel>
#include "processtreemodel.h"
class ProcessTreeFilterModel: public QSortFilterProxyModel
{
    Q_OBJECT
    std::map<ProcessTreeModel::Properties, QString> _searchStrings;
public:
    ProcessTreeFilterModel(QObject *parent);
    void setSearchText(const QString& text, int columnIndex)
    {
        _searchStrings.clear();
        if (columnIndex == -1)
        {
            _searchStrings[ProcessTreeModel::Properties::ProcessName] = text;
            _searchStrings[ProcessTreeModel::Properties::PID] = text;
            _searchStrings[ProcessTreeModel::Properties::ExecutablePath] = text;
        }
        else
        {
            _searchStrings[(ProcessTreeModel::Properties)columnIndex] = text;
        }
        invalidateRowsFilter();
    }
protected:
    bool lessThan( const QModelIndex & left, const QModelIndex & right ) const override;
    virtual bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const override;
};

#endif // PROCESSTREEFILTERMODEL_H
