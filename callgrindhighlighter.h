#ifndef CALLGRINDHIGHLIGHTER_H
#define CALLGRINDHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextCharFormat>

class CallgrindHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit CallgrindHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;
    QTextCharFormat fileFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat costFormat;
};

#endif // CALLGRINDHIGHLIGHTER_H
