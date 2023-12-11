#include "filterbycomboboxmodel.h"

FilterByComboBoxModel::FilterByComboBoxModel(QObject *parent)
    :QAbstractListModel(parent)
{
    m_values = new QList<QPair<int,QString>>();
}

int FilterByComboBoxModel::rowCount(const QModelIndex &) const
{
    return m_values->count();
}

QVariant FilterByComboBoxModel::data( const QModelIndex &index, int role ) const
{
    QVariant value;

    switch ( role )
    {
    case Qt::DisplayRole: //string
    {
        value = this->m_values->value(index.row()).second;
    }
    break;

    case Qt::UserRole: //data
    {
        value = this->m_values->value(index.row()).first;
    }
    break;

    default:
        break;
    }

    return value;
}

void FilterByComboBoxModel::populate(QList<QPair<int,QString>> *newValues)
{
    int idx = m_values->count();
    this->beginInsertRows(QModelIndex(), idx, idx);
    this->m_values = newValues;
    endInsertRows();
}
