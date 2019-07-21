#ifndef PAPERSTRIPWINDOW_H
#define PAPERSTRIPWINDOW_H

#include <QMainWindow>

class MidiFile;
class PaperStripWidget;
class QSpinBox;

class PaperStripWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit PaperStripWindow(QWidget *parent = nullptr);
    PaperStripWidget *mainWidget() { return m_mainWidget; }
    void setFile(MidiFile* file);

signals:

public slots:
    void print();

private:
    PaperStripWidget *m_mainWidget;
    QSpinBox *m_msPerBeatSB;
};

#endif // PAPERSTRIPWINDOW_H
