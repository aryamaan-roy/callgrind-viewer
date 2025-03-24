#include "textedit.h"
#include "linenumbering.h"
#include "callgrindhighlighter.h"
#include "foldoverlay.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QResizeEvent>
#include <QEvent>
#include <QMouseEvent>
#include <QTextBlock>
#include <QTextCursor>
#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QTextEdit>


TextEdit::TextEdit(QWidget *parent)
    : QPlainTextEdit(parent)
{
    setReadOnly(true);

    // line number area widget, install an event filter.
    lineNumberArea = new QWidget(this);
    lineNumberArea->installEventFilter(this);

    // Connect signals to update the line number area.
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
    if (obj == lineNumberArea) {
        if (event->type() == QEvent::Paint) {
            lineNumberAreaPaintEvent(static_cast<QPaintEvent *>(event));
            return true;
        } else if (event->type() == QEvent::MouseButtonRelease) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            // Check if the click is within the arrow region (first 16 pixels).
            if (mouseEvent->pos().x() < 16) {
                int y = mouseEvent->pos().y();
                QTextBlock block = firstVisibleBlock();
                int top = static_cast<int>(blockBoundingGeometry(block)
                                               .translated(contentOffset()).top());
                while (block.isValid()) {
                    int bottom = top + static_cast<int>(blockBoundingRect(block).height());
                    if (y >= top && y < bottom) {
                        toggleFoldAtBlock(block.blockNumber());
                        break;
                    }
                    block = block.next();
                    top = bottom;
                }
                return true;
            }
        }
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

void TextEdit::toggleFoldAtBlock(int blockNumber) {
    QTextDocument *doc = document();
    QTextBlock headerBlock = doc->findBlockByNumber(blockNumber);
    if (!headerBlock.isValid())
        return;

    // Normalize header text: remove extra spaces.
    QString headerText = headerBlock.text().trimmed();
    QString normalizedHeader = headerText;
    normalizedHeader.remove(" ");
    // Only allow folding if the header starts with "fn=".
    if (!normalizedHeader.startsWith("fn="))
        return;

    // Use header block's document position as a stable key.
    qint64 key = headerBlock.position();

    // If an overlay for this header already exists, then unfold
    if (foldOverlays.contains(key)) {
        FoldOverlay *overlay = foldOverlays.take(key);
        overlay->deleteLater();
        viewport()->update();
        return;
    }

    // Determine the region to fold:
    // Start from the block immediately after the header.
    QTextBlock foldStart = headerBlock.next();
    if (!foldStart.isValid())
        return;

    // Do not fold if the very next block is a header (i.e. one-line function mapping).
    QString nextText = foldStart.text().trimmed();
    QString normalizedNext = nextText;
    normalizedNext.remove(" ");
    if (normalizedNext.startsWith("fn=") || normalizedNext.startsWith("fl="))
        return;

    // Find the last block before the next header.
    QTextBlock foldEnd = foldStart;
    while (true) {
        QTextBlock nextBlock = foldEnd.next();
        if (!nextBlock.isValid())
            break;
        QString nText = nextBlock.text().trimmed();
        QString normNText = nText;
        normNText.remove(" ");
        if (normNText.startsWith("fn=") || normNText.startsWith("fl="))
            break;
        foldEnd = nextBlock;
    }

    // Do not fold if the region spans only one block.
    if (foldEnd.blockNumber() == foldStart.blockNumber())
        return;

    // Compute the overlay geometry.
    int startY = static_cast<int>(blockBoundingGeometry(foldStart)
                                      .translated(contentOffset()).top());
    // To ensure we do not cover the next header, check if foldEnd.next() is a header.
    QTextBlock candidate = foldEnd.next();
    if (candidate.isValid()) {
        QString candText = candidate.text().trimmed();
        QString normCand = candText;
        normCand.remove(" ");
        if (normCand.startsWith("fn=") || normCand.startsWith("fl=")) {
            if (foldEnd.previous().isValid() && foldEnd.previous().blockNumber() >= foldStart.blockNumber())
                foldEnd = foldEnd.previous();
        }
    }
    int endY = static_cast<int>(blockBoundingGeometry(foldEnd)
                                    .translated(contentOffset()).bottom());
    int foldHeight = endY - startY;
    if (foldHeight <= 0)
        return;
    int numLinesFolded = foldEnd.blockNumber() - foldStart.blockNumber() + 1;

    // Create an overlay widget covering the computed region.
    FoldOverlay *overlay = new FoldOverlay(viewport());
    overlay->setFoldInfo(numLinesFolded);
    overlay->setGeometry(0, startY, viewport()->width(), foldHeight);
    overlay->show();

    // Store the overlay in the hash
    foldOverlays.insert(key, overlay);

    // When the overlay is clicked, remove it
    connect(overlay, &FoldOverlay::clicked, this, [this, key]() {
        if (foldOverlays.contains(key)) {
            FoldOverlay *ov = foldOverlays.take(key);
            ov->deleteLater();
            viewport()->update();
        }
    });
}


void TextEdit::findText(const QString &pattern) {

    QList<QTextEdit::ExtraSelection> extraSelections;
    if (!pattern.isEmpty()) {
        QTextDocument *doc = this->document();
        QRegularExpression regex(pattern, QRegularExpression::CaseInsensitiveOption);
        QString text = doc->toPlainText();
        QRegularExpressionMatchIterator i = regex.globalMatch(text);
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            int start = match.capturedStart();
            int length = match.capturedLength();
            QTextCursor cursor(doc);
            cursor.setPosition(start);
            cursor.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, length);
            QTextEdit::ExtraSelection selection;
            selection.cursor = cursor;
            selection.format.setBackground(Qt::yellow); // highlight color
            extraSelections.append(selection);
        }
    }
    this->setExtraSelections(extraSelections);
}
