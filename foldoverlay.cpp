#include "foldoverlay.h"
#include <QPainter>
#include <QMouseEvent>

FoldOverlay::FoldOverlay(QWidget *parent)
    : QWidget(parent), m_numLines(0)
{
    // overlay is opaque.
    setAttribute(Qt::WA_NoSystemBackground, false);
    // white background
    setStyleSheet("background-color: white;");
}

void FoldOverlay::setFoldInfo(int numLines) {
    m_numLines = numLines;
    update();
}

void FoldOverlay::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(rect(), palette().window());
    painter.setPen(Qt::black);
    // right arrow with folded number of lines
    QString text = QString("► [Folded %1 lines]").arg(m_numLines);
    // add a left margin (e.g. 5 pixels)
    QRect textRect = rect().adjusted(5, 0, 0, 0);
    painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text);
}

void FoldOverlay::mousePressEvent(QMouseEvent *event) {
    Q_UNUSED(event);
    emit clicked();
}
