#ifndef TreeItem_H
#define TreeItem_H

#include <limits>
#include <QStandardItem>
#include <Windows.h>
class ProcessTreeItem
{
    unsigned int m_id;
    QString m_name;
    SIZE_T m_workingSetSize;
    SIZE_T m_pageFileUsage;
    ProcessTreeItem* m_parent;
    QList<ProcessTreeItem*> m_children;
    QList<QVariant> m_itemData;
    QString m_description;
    bool m_isReadyToDelete;
    QString m_executablePath;
    quint64 m_frequency;
    quint64 m_percentage;
    double m_cpuUsage;

    bool m_isDirty;
public:
    explicit ProcessTreeItem(unsigned int id, const QString &name, SIZE_T workingSetSize, SIZE_T pageFileUsage, ProcessTreeItem *parent = nullptr);
    explicit ProcessTreeItem(const QList<QVariant> &data, ProcessTreeItem *parent = 0);
    ~ProcessTreeItem();

    unsigned int getId() const { return m_id; }

    bool isDirty(){return m_isDirty;}
    void resetDirty(){m_isDirty = false;}

    QString getName() const { return m_name; }
    void setName(QString name)
    {
        m_isDirty = m_isDirty || (m_name != name);
        m_name = name;
    }

    QString getDescription() const { return m_description; }
    void setDescription(QString description)
    {
        m_isDirty = m_isDirty ||  (m_description != m_description);
        m_description = description;
    }

    QString getExecutablePath() const { return m_executablePath; }
    void setExecutablePath(QString executablePath) {m_executablePath = executablePath; }

    quint64 getFrequency() const { return m_frequency; }
    void setFrequency(quint64 frequency) {m_frequency = frequency; }

    quint64 getPercentage() const { return m_percentage; }
    void setPercentage(quint64 percentage) {m_percentage = percentage; }

    double getCpuUsage() const { return m_cpuUsage; }
    void setCpuUsage(double cpuUsage)
    {
        auto delta{fabs(m_cpuUsage - cpuUsage)};
        m_isDirty = m_isDirty || (delta > std::numeric_limits<double>::epsilon());
        m_cpuUsage = cpuUsage;
    }

    SIZE_T getWorkingSetSize() const { return m_workingSetSize; }
    void setWorkingSetSize(SIZE_T workingSetSize)
    {
        m_isDirty = m_isDirty ||  (m_workingSetSize != workingSetSize);
        m_workingSetSize = workingSetSize;
    }

    SIZE_T getPageFileUsage() const { return m_pageFileUsage; }
    void setPageFileUsage(SIZE_T pageFileUsage)
    {
        m_isDirty = m_isDirty || (m_pageFileUsage != pageFileUsage);
        m_pageFileUsage = pageFileUsage;
    }

    bool isReadyToDelete() const { return m_isReadyToDelete; }
    void setReadyToDelete()
    {
        m_isReadyToDelete = true;
    }

    void getIdsWithChildren(QSet<SIZE_T>& ids)
    {
        ids.insert(m_id);
        for (const auto& child : m_children)
        {
            child->getIdsWithChildren(ids);
        }
    }

    QSet<SIZE_T> getIdsWithChildren()
    {
        QSet<SIZE_T> ids;
        ids.insert(m_id);
        for (const auto& child : m_children)
        {
            child->getIdsWithChildren(ids);
        }
        return ids;
    }
    void setProperty(std::function<void(ProcessTreeItem&)> setter){setter(*this);}
    ProcessTreeItem *getParent() const { return m_parent; }
    void setParent(ProcessTreeItem* parent)
    { m_parent = parent; }

    ProcessTreeItem *getChildAt(int row) const { return m_children.value(row); }
    ProcessTreeItem* getChildById(SIZE_T id);
    bool contains(SIZE_T id);
    int getRowOfChild(ProcessTreeItem *child) const { return m_children.indexOf(child); }
    int getChildCount() const { return m_children.count(); }
    bool hasChildren() const { return !m_children.isEmpty(); }
    QList<ProcessTreeItem*> getChildren() const { return m_children; }

    void addChild(ProcessTreeItem *item);
    void insertChild(int row, ProcessTreeItem *item) { item->m_parent = this; m_children.insert(row, item); }
    void swapChildren(int oldRow, int newRow) { m_children.swapItemsAt(oldRow, newRow); }
    ProcessTreeItem* removeChild(int row);
    bool removeChildById(SIZE_T id);
    void clearChildren();

    int getRow() const;

};

#endif // TreeItem_H
