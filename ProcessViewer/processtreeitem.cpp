#include "processtreeitem.h"

ProcessTreeItem::ProcessTreeItem(unsigned int id, const QString &name, SIZE_T workingSetSize, SIZE_T pageFileUsage, ProcessTreeItem *parent):
    m_id(id),
    m_name(name),
    m_workingSetSize(workingSetSize),
    m_pageFileUsage(pageFileUsage),
    m_parent(parent),
    m_isReadyToDelete(false)
{

}

void ProcessTreeItem::addChild(ProcessTreeItem *item)
{
    item->m_parent = this;
    m_children << item;
    qDebug() << "Set parent" << this->m_id << "(" << getName() << ")" " for " << item->m_id << "(" << item->m_name << ")";
}

ProcessTreeItem::ProcessTreeItem(const QList<QVariant> &data, ProcessTreeItem *parent):
    m_parent(parent), m_itemData(data), m_id(7777777),m_isReadyToDelete(false)
{

}

ProcessTreeItem* ProcessTreeItem::removeChild(int row)
{
    auto* item = m_children.takeAt(row);
    Q_ASSERT(item);
    item->m_parent = nullptr;
    return item;
}

void ProcessTreeItem::clearChildren()
{
    m_children.clear();
    qDebug() << "Cleaned children list of " << m_id;
}

int ProcessTreeItem::getRow() const
{
    return m_parent->m_children.indexOf(const_cast<ProcessTreeItem*>(this));
}

ProcessTreeItem* ProcessTreeItem::getChildById(SIZE_T id)
{
    auto childIt = std::find_if(m_children.begin(), m_children.end(), [=](ProcessTreeItem* someclass) { return someclass->getId() == id; });
    return childIt != m_children.end() ? *childIt : nullptr;
}
bool ProcessTreeItem::contains(SIZE_T id)
{
    for(const auto& child:m_children)
    {
        if (child->getId() == id)
        {
            return true;
        }
    }
    return false;
}

bool ProcessTreeItem::removeChildById(SIZE_T id)
{
    m_children.removeIf([=](ProcessTreeItem* it){return (*it).getId() == id;});
    return true;
}

ProcessTreeItem::~ProcessTreeItem()
{
    qDebug() << "Deallocation of " << m_id << m_name;
}
