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

class PaperStripWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PaperStripWidget(QWidget *parent = nullptr);
    void setFile(MidiFile* file);

signals:

public slots:
    void onPointedToTime(int ms);

protected:
    void paintEvent(QPaintEvent* event);
    void paint30Note(QPainter* painter);
    void paintChannel(QPainter *painter, int channel);
    int xPosOfMs(int ms);
    int yPosOfLine(int line);

private:
    MidiFile* file;
    int m_verticalSpacing = 12;
    int m_horizontalSpacing = 36; // distance between solid vertical lines
    int m_stripLength = 100; // number of solid vertical lines
    int m_gridLeftX = 50;
    int m_gridTopY = 20;
    int m_topLine = 41;
    int m_msPerBeat = 400; // time between solid vertical lines (there are dashed lines between them, so we can have half-beats too)
    int m_pointedToTime = -1;
};

#endif // PAPERSTRIPWIDGET_H
