#ifndef PROCESSTABLEFILTERMODEL_H
#define PROCESSTABLEFILTERMODEL_H

#include <QSortFilterProxyModel>
#include "processtablemodel.h"
class ProcessTableFilterModel: public QSortFilterProxyModel
{
    Q_OBJECT
    std::map<ProcessTableModel::Properties, QString> _searchStrings;
public:
    ProcessTableFilterModel(QObject *parent);
    void setSearchText(const QString& text, int columnIndex)
    {
        _searchStrings.clear();
        if (columnIndex == -1)
        {
            _searchStrings[ProcessTableModel::Properties::ProcessName] = text;
            _searchStrings[ProcessTableModel::Properties::PID] = text;
            _searchStrings[ProcessTableModel::Properties::ExecutablePath] = text;
        }
        else
        {
            _searchStrings[(ProcessTableModel::Properties)columnIndex] = text;
        }
        invalidateRowsFilter();
    }
protected:
    bool lessThan( const QModelIndex & left, const QModelIndex & right ) const override;
    virtual bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const override;
};

#endif // PROCESSTABLEFILTERMODEL_H
