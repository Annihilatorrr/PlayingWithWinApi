#include "processpropertiesdialog.h"
#include <QVBoxLayout>
#include <QPushButton>
#include "imagetab.h"

ProcessPropertiesDialog::ProcessPropertiesDialog(const ProcessProperties& model, QWidget* parent): QDialog(parent), m_model(model)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(QString(tr("%1:%2 Properties")).arg(model.processName).arg(model.processId));
    m_centralWidget = new QTabWidget(this);
    ImageTab* imageTab = new ImageTab();
    m_centralWidget->addTab(imageTab, tr("Image"));
    QVBoxLayout* layout = new QVBoxLayout();

    layout->addWidget(m_centralWidget);
    int nWidth = 300;
    int nHeight = 400;
    if (parent != NULL)
    {
        setGeometry(parent->x() + parent->width()/2 - nWidth/2,
                    parent->y() + parent->height()/2 - nHeight/2,
                    nWidth, nHeight);
    }
    else
    {
        resize(nWidth, nHeight);
    }

    setLayout(layout);
}


ProcessPropertiesDialog::~ProcessPropertiesDialog()
{
    qDebug() << "Window deleted for process" << m_model.processId << m_model.processName;
}
