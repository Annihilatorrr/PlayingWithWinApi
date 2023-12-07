#include "processtreeviewitemdelegate.h"

#include <QApplication>

ProcessTreeViewItemDelegate::ProcessTreeViewItemDelegate()
{

}


void ProcessTreeViewItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                     const QModelIndex &index) const
{
    if (!index.isValid())
        return;

    QStyleOptionViewItem opt = option;
    //initStyle(&opt, index);
    int padding = 3;

    painter->save();
    painter->setClipRect(opt.rect);
    opt.rect = opt.rect.adjusted(padding, padding, -padding, -padding);
    painter->drawText(opt.rect, Qt::AlignLeft | Qt::AlignVCenter,
                      opt.fontMetrics.elidedText(opt.text, Qt::ElideRight,
                                                 opt.rect.width()));
    painter->restore();

    opt.rect = option.rect;
    opt.textElideMode = Qt::ElideNone;
    opt.text = "";

    QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter);
}
