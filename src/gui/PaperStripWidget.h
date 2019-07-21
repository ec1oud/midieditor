/*
 * Music box strip renderer for MidiEditor
 * Copyright (C) 2019 Shawn Rutledge
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PAPERSTRIPWIDGET_H
#define PAPERSTRIPWIDGET_H

#include <QWidget>

class MidiFile;
class QPrinter;

class PaperStripWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int msPerBeat READ msPerBeat WRITE setMsPerBeat NOTIFY msPerBeatChanged)
    Q_PROPERTY(int offsetHalfBeats READ offsetHalfBeats WRITE setOffsetHalfBeats NOTIFY offsetHalfBeatsChanged)

public:
    explicit PaperStripWidget(QWidget *parent = nullptr);
    void setFile(MidiFile* m_file);

    QSize sizeHint() const;
    int msPerBeat() const;
    int offsetHalfBeats() const;

    void print(QPrinter *printer);

public slots:
    void setMsPerBeat(int msPerBeat);
    void setOffsetHalfBeats(int offsetHalfBeats);
    void onPointedToTime(int ms);

signals:
    void msPerBeatChanged(int msPerBeat);
    void offsetHalfBeatsChanged(int offsetHalfBeats);

protected:
    void paintEvent(QPaintEvent* event);
    void paint30Note(QPainter* painter);
    void paintChannel(QPainter *painter, int channel);
    int xPosOfMs(int ms);
    int yPosOfLine(int line);

private:
    MidiFile* m_file = nullptr;
    int m_verticalSpacing = 12;
    int m_horizontalSpacing = 48; // distance between solid vertical lines
    int m_stripLength = 100; // number of solid vertical lines
    int m_gridLeftX = 50;
    int m_gridTopY = 20;
    int m_topLine = 41;
    int m_msPerBeat = 400; // time between solid vertical lines (there are dashed lines between them, so we can have half-beats too)
    int m_pointedToTime = -1;
    int m_offsetHalfBeats = 0;
};

#endif // PAPERSTRIPWIDGET_H
