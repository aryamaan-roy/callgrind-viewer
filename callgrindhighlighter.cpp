#include "callgrindhighlighter.h"

CallgrindHighlighter::CallgrindHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    fileFormat.setForeground(Qt::blue);        // Lines starting with "fl="
    functionFormat.setForeground(Qt::darkGreen); // Lines starting with "fn="
    costFormat.setForeground(Qt::red);           // Numeric cost lines

    // Rule: File name lines (starting with "fl=")
    HighlightingRule rule;
    rule.pattern = QRegularExpression("^fl=.*");
    rule.format = fileFormat;
    highlightingRules.append(rule);

    // Rule: Function name lines (starting with "fn=")
    rule.pattern = QRegularExpression("^fn=.*");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    // Rule: Lines beginning with a number (cost information)
    rule.pattern = QRegularExpression("^[0-9]+");
    rule.format = costFormat;
    highlightingRules.append(rule);
}

void CallgrindHighlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
        QRegularExpressionMatchIterator i = rule.pattern.globalMatch(text);
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
