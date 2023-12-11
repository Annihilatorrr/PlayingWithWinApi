#ifndef PROCESSPROPERTIESDIALOG_H
#define PROCESSPROPERTIESDIALOG_H

#include <QDialog>
#include <QTabWidget>
#include "processproperties.h"

class ProcessPropertiesDialog : public QDialog
{
    Q_OBJECT
private:
    QTabWidget* m_centralWidget;
    ProcessProperties m_model;

public:
    explicit ProcessPropertiesDialog(const ProcessProperties& model, QWidget *parent=nullptr);
    ~ProcessPropertiesDialog();
};

#endif // PROCESSPROPERTIESDIALOG_H
