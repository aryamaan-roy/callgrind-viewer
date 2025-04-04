#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QPlainTextEdit>
#include <QUrl>
#include <QHash>

class LineNumberHelper;
class FoldOverlay;

class TextEdit : public QPlainTextEdit {
    Q_OBJECT
public:
    explicit TextEdit(QWidget *parent = nullptr);
    void setContents(const QString &fileName);
    QVariant loadResource(int type, const QUrl &name) override;
    int lineNumberAreaWidth();

signals:
    void fileNameChanged(const QString &fileName);

protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

public slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &rect, int dy);
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    void toggleFoldAtBlock(int blockNumber);
    void findText(const QString &pattern);

private:
    QUrl srcUrl;
    QWidget *lineNumberArea;

    // hash to store fold overlays for each header block, keyed by header block position.
    QHash<qint64, FoldOverlay*> foldOverlays;

    // LineNumberHelper access to protected members.
    friend class LineNumberHelper;
};

#endif // TEXTEDIT_H
