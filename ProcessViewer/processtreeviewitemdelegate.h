#ifndef PROCESSTREEVIEWITEMDELEGATE_H
#define PROCESSTREEVIEWITEMDELEGATE_H

#include <QPainter>
#include <QStyleOptionViewItem>

class ProcessTreeViewItemDelegate
{
public:

    ProcessTreeViewItemDelegate();
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
};

#endif // PROCESSTREEVIEWITEMDELEGATE_H
