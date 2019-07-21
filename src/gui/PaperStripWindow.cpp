#include "PaperStripWindow.h"
#include "PaperStripWidget.h"

#include "../midi/MidiFile.h"

#include <QLabel>
#include <QPrintDialog>
#include <QPrinter>
#include <QScrollArea>
#include <QSpinBox>
#include <QToolBar>

PaperStripWindow::PaperStripWindow(QWidget *parent) : QMainWindow(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Music Box Strip"));
    resize(800, 450);
    m_mainWidget = new PaperStripWidget(this);
    QScrollArea *scroll = new QScrollArea(this);
    scroll->setWidget(m_mainWidget);
    setCentralWidget(scroll);

    QToolBar *tb = addToolBar(tr("Tools for Music Box Strip"));
    QAction *printAction = new QAction(QIcon(":/run_environment/graphics/tool/print.png"), tr("Print"), this);
    tb->addAction(printAction);
    connect(printAction, &QAction::triggered, this, &PaperStripWindow::print);

    tb->addSeparator();

    tb->addWidget(new QLabel(tr("ms per beat")));
    m_msPerBeatSB = new QSpinBox(this);
    m_msPerBeatSB->setRange(10, 10000);
    m_msPerBeatSB->setSingleStep(100);
    m_msPerBeatSB->setValue(400);
    tb->addWidget(m_msPerBeatSB);
    connect(m_msPerBeatSB, SIGNAL(valueChanged(int)),
            m_mainWidget, SLOT(setMsPerBeat(int)));

    tb->addWidget(new QLabel(tr("offset")));
    QSpinBox *offsetSB = new QSpinBox(this);
    offsetSB->setRange(-100, 100);
    offsetSB->setSingleStep(1);
    offsetSB->setValue(0);
    tb->addWidget(offsetSB);
    connect(offsetSB, SIGNAL(valueChanged(int)),
            m_mainWidget, SLOT(setOffsetHalfBeats(int)));
}

void PaperStripWindow::setFile(MidiFile *file)
{
//    m_msPerBeatSB->setValue(  TODO set the tempo somehow
    m_mainWidget->setFile(file);
}

void PaperStripWindow::print()
{
    QPrinter printer(QPrinter::ScreenResolution);
    printer.setPageOrientation(QPageLayout::Landscape);
    QPrintDialog printDialog(&printer, this);
    if (printDialog.exec() == QDialog::Accepted) {
        m_mainWidget->print(&printer);
    }
}
