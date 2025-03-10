#include "textedit.h"
#include "linenumbering.h"
#include "callgrindhighlighter.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QResizeEvent>
#include <QEvent>

TextEdit::TextEdit(QWidget *parent)
    : QPlainTextEdit(parent)
{
    setReadOnly(true);

    // line number area widget and install event filter
    lineNumberArea = new QWidget(this);
    lineNumberArea->installEventFilter(this);

    // signals to update the line number area
    connect(this, &QPlainTextEdit::blockCountChanged,
            this, &TextEdit::updateLineNumberAreaWidth);
    connect(this, &QPlainTextEdit::updateRequest,
            this, &TextEdit::updateLineNumberArea);

    updateLineNumberAreaWidth(0);
}

int TextEdit::lineNumberAreaWidth() {
    return LineNumberHelper::calculateLineNumberAreaWidth(this);
}

void TextEdit::updateLineNumberAreaWidth(int /* newBlockCount */) {
    LineNumberHelper::updateLineNumberAreaWidth(this, lineNumberArea);
}

void TextEdit::updateLineNumberArea(const QRect &rect, int dy) {
    LineNumberHelper::updateLineNumberArea(this, lineNumberArea, rect, dy);
}

void TextEdit::resizeEvent(QResizeEvent *event) {
    QPlainTextEdit::resizeEvent(event);
    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(),
                                      lineNumberAreaWidth(), cr.height()));
}

bool TextEdit::eventFilter(QObject *obj, QEvent *event) {
    if (obj == lineNumberArea && event->type() == QEvent::Paint) {
        lineNumberAreaPaintEvent(static_cast<QPaintEvent *>(event));
        return true;
    }
    return QPlainTextEdit::eventFilter(obj, event);
}

void TextEdit::lineNumberAreaPaintEvent(QPaintEvent *event) {
    LineNumberHelper::paintLineNumbers(this, lineNumberArea, event);
}

void TextEdit::setContents(const QString &fileName)
{
    const QFileInfo fi(fileName);
    srcUrl = QUrl::fromLocalFile(fi.absoluteFilePath());
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        QString data = in.readAll();
        setPlainText(data);
        // Detect Callgrind files
        if (data.contains("# callgrind format"))
            new CallgrindHighlighter(document());
    }
    emit fileNameChanged(fileName);
}

QVariant TextEdit::loadResource(int type, const QUrl &name)
{
    if (type == QTextDocument::ImageResource) {
        QFile file(srcUrl.resolved(name).toLocalFile());
        if (file.open(QIODevice::ReadOnly))
            return file.readAll();
    }
    return QPlainTextEdit::loadResource(type, name);
}
