#ifndef CALLGRINDHIGHLIGHTER_H
#define CALLGRINDHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextCharFormat>
#include <QVector>

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

    // Formats for header tokens:
    QTextCharFormat descFormat;     // desc:
    QTextCharFormat cmdFormat;      // cmd:
    QTextCharFormat partFormat;     // part:
    QTextCharFormat eventsFormat;   // events:
    QTextCharFormat summaryFormat;  // summary:

    // Formats for file and function tokens:
    QTextCharFormat fileFormat;     // fl=
    QTextCharFormat functionFormat; // fn=
    QTextCharFormat cflFormat;      // cfl=
    QTextCharFormat cfnFormat;      // cfn=
    QTextCharFormat cfiFormat;      // cfi=
    QTextCharFormat callsFormat;    // calls=

    // Format for numeric cost lines:
    QTextCharFormat costFormat;

    // Format for comment lines:
    QTextCharFormat commentFormat;
};

#endif // CALLGRINDHIGHLIGHTER_H
