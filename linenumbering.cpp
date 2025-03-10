#include "linenumbering.h"
#include "textedit.h"
#include <QPainter>
#include <QTextBlock>
#include <QFontMetrics>
#include <algorithm>

int LineNumberHelper::calculateLineNumberAreaWidth(TextEdit *editor) {
    int digits = 1;
    int max = std::max(1, editor->blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    int space = 3 + editor->fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    return space;
}

void LineNumberHelper::updateLineNumberAreaWidth(TextEdit *editor, QWidget *lineNumberArea) {
    Q_UNUSED(lineNumberArea);
    editor->setViewportMargins(calculateLineNumberAreaWidth(editor), 0, 0, 0);
}

void LineNumberHelper::updateLineNumberArea(TextEdit *editor, QWidget *lineNumberArea, const QRect &rect, int dy) {
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(editor->viewport()->rect()))
        updateLineNumberAreaWidth(editor, lineNumberArea);
}

void LineNumberHelper::paintLineNumbers(TextEdit *editor, QWidget *lineNumberArea, QPaintEvent *event) {
    QPainter painter(lineNumberArea);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // background color
    QColor backgroundColor(200, 200, 210);
    painter.fillRect(event->rect(), backgroundColor);

    // text color for line numbers
    QColor textColor(70, 70, 70);
    painter.setPen(textColor);

    QTextBlock block = editor->firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = static_cast<int>(editor->blockBoundingGeometry(block)
                                   .translated(editor->contentOffset()).top());
    int bottom = top + static_cast<int>(editor->blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.drawText(0, top, lineNumberArea->width() - 5,
                             editor->fontMetrics().height(),
                             Qt::AlignRight, number);
        }
        block = block.next();
        top = bottom;
        bottom = top + static_cast<int>(editor->blockBoundingRect(block).height());
        ++blockNumber;
    }
}
