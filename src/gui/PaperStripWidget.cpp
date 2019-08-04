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

#include "PaperStripWidget.h"
#include "../midi/MidiChannel.h"
#include "../midi/MidiFile.h"
#include "../MidiEvent/NoteOnEvent.h"
#include "../protocol/Protocol.h"
#include <QDebug>
#include <QPainter>
#include <QPaintEvent>
#include <QPainterPath>
#include <QPrinter>

PaperStripWidget::PaperStripWidget(QWidget *parent) : QWidget(parent)
{
}

void PaperStripWidget::setFile(MidiFile *f)
{
    m_file = f;
    connect(m_file->protocol(), SIGNAL(actionFinished()), this, SLOT(update()));
    updateSize();
}

int PaperStripWidget::msPerBeat() const
{
    return m_msPerBeat;
}

void PaperStripWidget::setMsPerBeat(int msPerBeat)
{
    if (m_msPerBeat == msPerBeat)
        return;

    m_msPerBeat = msPerBeat;
    emit msPerBeatChanged(m_msPerBeat);
    updateSize();
}

int PaperStripWidget::offsetHalfBeats() const
{
    return m_offsetHalfBeats;
}

void PaperStripWidget::setOffsetHalfBeats(int offsetHalfBeats)
{
    if (m_offsetHalfBeats == offsetHalfBeats)
        return;

    m_offsetHalfBeats = offsetHalfBeats;
    emit offsetHalfBeatsChanged(m_offsetHalfBeats);
    updateSize();
}

void PaperStripWidget::onPointedToTime(int ms)
{
    m_pointedToTime = ms;
    update();
}

void PaperStripWidget::updateSize()
{
    m_stripLength = m_file->maxTime() * 2 / m_msPerBeat + m_offsetHalfBeats;
    int fullBeats = (m_stripLength + 1) / 2;
    resize(fullBeats * m_horizontalSpacing + m_gridLeftX + 10,
           m_verticalSpacing * 30 + m_gridTopY + 10);
    update();
}

void PaperStripWidget::paintEvent(QPaintEvent *event)
{
    QPainter* painter = new QPainter(this);
    painter->setClipRect(event->rect());
    paint30Note(painter); // TODO make configurable
    delete painter;
}

void PaperStripWidget::print(QPrinter *printer)
{
    QPainter painter;
    printer->setResolution(152);
    painter.begin(printer);
    int halfBeatsPerPage = m_stripPageLength * 2; // painter.viewport().width() / m_horizontalSpacing * 2;
    int pageWidth = m_stripPageLength * m_horizontalSpacing;
    qDebug() << "first page:" << painter.viewport() << "res" << printer->resolution()
             << "half beats across width:" << halfBeatsPerPage << "of total" << m_stripLength << "pixels width:" << pageWidth;
    int pageCount = 0;
    int halfPageHeight = painter.viewport().height() / 3;
    painter.translate(pageWidth, halfPageHeight * 2);
    for (int offset = m_offsetHalfBeats; offset < m_stripLength; offset += halfBeatsPerPage) {
        if (pageCount % 3) {
            painter.translate(-pageWidth, halfPageHeight);
        } else {
            painter.translate(-pageWidth, halfPageHeight * -2);
            if (pageCount)
                printer->newPage();
        }
        qDebug() << "page" << pageCount << "rect" << printer->pageRect(QPrinter::DevicePixel) << "offset" << offset << "viewport" << painter.viewport();
        paint30Note(&painter);
        ++pageCount;
    }
    painter.end();
}

void PaperStripWidget::paint30Note(QPainter *painter)
{
    const QVector<QString> notes = { "E\u2083", "D\u2083", "C\u2083",
                                     "B\u2082", "A\u266F\u2082", "A\u2082", "G\u266F\u2082", "G\u2082",
                                     "F\u266F\u2082", "F\u2082", "E\u2082", "D\u266F\u2082", "D\u2082",
                                     "C\u266F\u2082", "C\u2082",
                                     "B\u2081", "A\u266F\u2081", "A\u2081", "G\u266F\u2081", "G\u2081",
                                     "F\u266F\u2081", "F\u2081", "E\u2081", "D\u2081", "C\u2081",
                                     "B", "A", "G", "D", "C"
                                   };
    int fullBeats = (m_stripLength + 1) / 2;
    int lenX = fullBeats * m_horizontalSpacing;
    int widthY = m_gridTopY + 29 * m_verticalSpacing;
    // draw grid
    painter->setPen(QPen(Qt::cyan, 3.0));
    for (int i = m_stripPageLength; i <= fullBeats; i += m_stripPageLength) {
        int x = m_gridLeftX + i * m_horizontalSpacing;
        painter->drawLine(x, m_gridTopY - 10, x, widthY + 10);
        painter->drawText(x + 4, m_gridTopY, QString::number(1 + (i / m_stripPageLength)));
    }
    painter->setPen(Qt::lightGray);
    for (int i = 0; i < 30; ++i) {
        int y = m_gridTopY + i * m_verticalSpacing;
        painter->drawLine(m_gridLeftX, y, m_gridLeftX + lenX, y);
    }
    for (int i = 0; i <= fullBeats; ++i) {
        int x = m_gridLeftX + i * m_horizontalSpacing;
        painter->drawLine(x, m_gridTopY, x, widthY);
    }
    painter->setPen(QPen(Qt::lightGray, 1, Qt::DashLine));
    for (int i = 0; i <= fullBeats - 1; ++i) {
        int x = m_gridLeftX + (m_horizontalSpacing / 2) + (i * m_horizontalSpacing);
        painter->drawLine(x, m_gridTopY, x, widthY);
    }
    // draw the file path
    painter->setPen(Qt::black);
    for (int i = 0; i <= fullBeats; i += m_stripPageLength * 3) {
        int x = 40 + m_gridLeftX + i * m_horizontalSpacing;
        painter->drawText(x, m_gridTopY - 4, m_file->path());
    }

    // draw holes
    for (int i = 0; i < 19; i++)
        paintChannel(painter, i);

    int fontHeight = painter->fontMetrics().ascent();
    // mark the C's
    // C-1 is line 127, C9 is line 7; but we don't have that much range
    painter->setPen(Qt::blue);
    for (int i = 79; i > 42; i -= 12) {
        int y = yPosOfLine(i) + fontHeight / 2;
        // qDebug() << QString("C%1").arg(((127 - i) / 12) - 1) << i << "y" << y;
        painter->drawText(2, y, QString("C%1").arg(((127 - i) / 12) - 1));
    }
    // draw the pointed-to time
    if (m_pointedToTime >= 0) {
        painter->setPen(Qt::red);
        int x = xPosOfMs(m_pointedToTime);
        painter->drawLine(x, 0, x, height());
    }
    // draw labels like those on the paper strip
    painter->setPen(Qt::black);
    painter->rotate(-90);
    for (int i = 0; i < 30; ++i) {
        int y = m_gridTopY + i * m_verticalSpacing;
        painter->drawText(-y - m_verticalSpacing / 2, m_gridLeftX + 12 * (i % 2) - 32,
                          m_verticalSpacing, m_horizontalSpacing / 2,
                          Qt::AlignHCenter | Qt::TextDontClip, notes[i]);
    }
    painter->rotate(90);
}

void PaperStripWidget::paintChannel(QPainter *painter, int channel)
{
    if (!m_file->channel(channel)->visible())
        return;
    QMultiMap<int, MidiEvent*>* map = m_file->channelEvents(channel);
    QMap<int, MidiEvent*>::iterator it = map->begin();
    QPainterPath ellipse;
    ellipse.addEllipse(0, 0, 9, 9);
    QColor color = *m_file->channel(channel)->color();
    int minTick = 0;
    int maxTick = 0;
    {
        auto ticks = map->keys();
        if (!ticks.isEmpty()) {
            minTick = map->keys().first();
            maxTick = map->keys().last();
        }
    }
    int minX = std::numeric_limits<int>::max();
    int minY = std::numeric_limits<int>::max();
    int maxX = std::numeric_limits<int>::min();
    int maxY = std::numeric_limits<int>::min();
    while (it != map->end()) { // && it.key() <= endTick) {
        MidiEvent* event = it.value();
        if (OnEvent* onEvent = dynamic_cast<OnEvent*>(event)) {
            if (event->line() > 127)
                continue; // not a musical note
            int x = xPosOfMs(m_file->msOfTick(event->midiTime()));
            int y = yPosOfLine(event->line());
            minX = qMin(x, minX);
            minY = qMin(y, minY);
            maxX = qMax(x, maxX);
            maxY = qMax(y, maxY);
            if (x >= m_gridLeftX && y >= m_gridTopY) {
                QPainterPath ell = ellipse.translated(x - 4, y - 4);
                painter->fillPath(ell, color);
                painter->drawPath(ell);
            }
        }
        ++it;
    }
    if (minX < std::numeric_limits<int>::max()) // we drew something
        qDebug() << channel << color.name() << "offset" << m_offsetHalfBeats << "range: ticks" << minTick << maxTick << "x" << minX << maxX << "y" << minY << maxY;
}

int PaperStripWidget::xPosOfMs(int ms)
{
    return m_gridLeftX + qRound(qreal(ms) / m_msPerBeat * m_horizontalSpacing) +
            m_offsetHalfBeats * m_horizontalSpacing / 2;
}

int PaperStripWidget::yPosOfLine(int line)
{
    // negative means the note doesn't exist
    int stripLine = -1000;
    // special cases: zones of the strip with no sharps or flats
    switch (line) {
    case 39: // E3 on the strip
        stripLine = 0;
        break;
    case 41: // D3 on the strip
        stripLine = 1;
        break;
    case 65: // D1 on the strip
        stripLine = 23;
        break;
    case 67: // C4, labeled C1 on the strip
        stripLine = 24;
        break;
    case 68: // B on the strip
        stripLine = 25;
        break;
    case 70: // A on the strip
        stripLine = 26;
        break;
    case 72: // G on the strip
        stripLine = 27;
        break;
    case 77: // D on the strip
        stripLine = 28;
        break;
    case 79: // C on the strip
        stripLine = 29;
        break;
    default:
        // From E1 to C3 on the strip, inclusive, we have all sharps and flats available
        if (line < 64 && line > 42)
            stripLine = line - m_topLine;
        break;
    }
    return m_gridTopY + stripLine * m_verticalSpacing;
}
