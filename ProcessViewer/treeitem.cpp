#include "treeitem.h"

TreeItem::TreeItem(unsigned int id, const QString &name, SIZE_T workingSetSize, SIZE_T pageFileUsage, TreeItem *parent):
    m_id(id),
    m_name(name),
    m_workingSetSize(workingSetSize),
    m_pageFileUsage(pageFileUsage),
    m_parent(parent),
    m_isReadyToDelete(false)
{

}

void TreeItem::addChild(TreeItem *item)
{
    item->m_parent = this;
    m_children << item;
    qDebug() << "Set parent" << this->m_id << "(" << getName() << ")" " for " << item->m_id << "(" << item->m_name << ")";
}

TreeItem::TreeItem(const QList<QVariant> &data, TreeItem *parent):
    m_parent(parent), m_itemData(data), m_id(0xffffffff),m_isReadyToDelete(false)
{

}

TreeItem* TreeItem::removeChild(int row)
{
    auto* item = m_children.takeAt(row);
    Q_ASSERT(item);
    item->m_parent = nullptr;
    return item;
}

void TreeItem::clearChildren()
{
    m_children.clear();
    qDebug() << "Cleaned children list of " << m_id;
}

int TreeItem::getRow() const
{
    return m_parent ? m_parent->m_children.indexOf(const_cast<TreeItem*>(this)) : 0;
}

TreeItem* TreeItem::getChildById(SIZE_T id)
{
    auto childIt = std::find_if(m_children.begin(), m_children.end(), [=](TreeItem* someclass) { return someclass->getId() == id; });
    //qDebug() <<  this->m_id << (childIt !=  m_childrenHashTable.end() ? "has":"does not have") << " children with id" << id;
    return childIt != m_children.end() ? *childIt : nullptr;
}

bool TreeItem::removeChildById(SIZE_T id)
{
    m_children.removeIf([=](TreeItem* it){return (*it).getId() == id;});
    return true;
}

TreeItem::~TreeItem()
{
    qDebug() << "Deallocation of " << m_id << m_name;
}
