#ifndef LINENUMBERING_H
#define LINENUMBERING_H

#include <QPlainTextEdit>
#include <QWidget>
#include <QPaintEvent>

class TextEdit;

class LineNumberHelper {
public:
    static int calculateLineNumberAreaWidth(TextEdit *editor);
    static void updateLineNumberAreaWidth(TextEdit *editor, QWidget *lineNumberArea);
    static void updateLineNumberArea(TextEdit *editor, QWidget *lineNumberArea, const QRect &rect, int dy);
    static void paintLineNumbers(TextEdit *editor, QWidget *lineNumberArea, QPaintEvent *event);
};

#endif // LINENUMBERING_H
