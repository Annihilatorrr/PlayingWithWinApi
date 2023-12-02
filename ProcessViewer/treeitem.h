#ifndef TreeItem_H
#define TreeItem_H

#include <QStandardItem>
#include <Windows.h>
class TreeItem
{
    unsigned int m_id;
    QString m_name;
    SIZE_T m_workingSetSize;
    SIZE_T m_pageFileUsage;
    TreeItem* m_parent;
    QList<TreeItem*> m_children;
    QList<QVariant> m_itemData;
    QString m_description;
    bool m_isReadyToDelete;
    QString m_executablePath;
    quint64 m_frequency;
    quint64 m_percentage;
    double m_cpuUsage;
public:
    explicit TreeItem(unsigned int id, const QString &name, SIZE_T workingSetSize, SIZE_T pageFileUsage, TreeItem *parent = nullptr);
    explicit TreeItem(const QList<QVariant> &data, TreeItem *parent = 0);
    ~TreeItem();

    unsigned int getId() const { return m_id; }

    QString getName() const { return m_name; }
    void setName(QString name) {m_name = name; }

    QString getDescription() const { return m_description; }
    void setDescription(QString description) {m_description = description; }

    QString getExecutablePath() const { return m_executablePath; }
    void setExecutablePath(QString executablePath) {m_executablePath = executablePath; }

    quint64 getFrequency() const { return m_frequency; }
    void setFrequency(quint64 frequency) {m_frequency = frequency; }

    quint64 getPercentage() const { return m_percentage; }
    void setPercentage(quint64 percentage) {m_percentage = percentage; }

    double getCpuUsage() const { return m_cpuUsage; }
    void setCpuUsage(double cpuUsage) {m_cpuUsage = cpuUsage; }

    SIZE_T getMorkingSetSize() const { return m_workingSetSize; }
    void setWorkingSetSize(SIZE_T workingSetSize) {m_workingSetSize = workingSetSize; }

    SIZE_T getPageFileUsage() const { return m_pageFileUsage; }
    void setPageFileUsage(SIZE_T pageFileUsage) {m_pageFileUsage = pageFileUsage; }

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
    void setProperty(std::function<void(TreeItem&)> setter){setter(*this);}
    TreeItem *getParent() const { return m_parent; }
    void setParent(TreeItem* parent) { m_parent = parent; }

    TreeItem *getChildAt(int row) const { return m_children.value(row); }
    TreeItem* getChildById(SIZE_T id);
    int getRowOfChild(TreeItem *child) const { return m_children.indexOf(child); }
    int getChildCount() const { return m_children.count(); }
    bool hasChildren() const { return !m_children.isEmpty(); }
    QList<TreeItem*> getChildren() const { return m_children; }

    void addChild(TreeItem *item);
    void insertChild(int row, TreeItem *item) { item->m_parent = this; m_children.insert(row, item); }
    void swapChildren(int oldRow, int newRow) { m_children.swapItemsAt(oldRow, newRow); }
    TreeItem* removeChild(int row);
    bool removeChildById(SIZE_T id);
    void clearChildren();

    int getRow() const;

};

#endif // TreeItem_H