#ifndef FILTERBYCOMBOBOXMODEL_H
#define FILTERBYCOMBOBOXMODEL_H

#include <QAbstractListModel>

class FilterByComboBoxModel : public QAbstractListModel
{
public:
    FilterByComboBoxModel(QObject *parent=nullptr);

    int rowCount(const QModelIndex &) const;
    QVariant data(const QModelIndex &index, int role) const;
    void populate(QList<QPair<int,QString>> *newValues);

private:
    QList<QPair<int,QString>>* m_values;
};

#endif // FILTERBYCOMBOBOXMODEL_H
