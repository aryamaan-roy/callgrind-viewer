#include "callgrindhighlighter.h"
#include <utility>

namespace {
// Helper function that builds and returns the cost-line regex pattern.
QString buildCostLinePattern() {
    // Optional sign: '+' or '-'
    QString signPart = "[+\\-]?";
    // A hexadecimal number, e.g., 0x40010020
    QString hexPart = "0[xX][0-9A-Fa-f]+";
    // A decimal number, e.g., 10
    QString decPart = "\\d+";
    // Either a hexadecimal or a decimal number
    QString numberPart = "(?:" + hexPart + "|" + decPart + ")";
    // Optional compression: '*' followed by one or more digits (e.g., *1)
    QString compressionPart = "(?:\\*\\d+)?";
    // A token is an optional sign, a number (hex or dec), and an optional compression part.
    QString tokenPattern = signPart + numberPart + compressionPart;
    // The full pattern: one or more tokens separated by whitespace.
    return "^\\s*(?:" + tokenPattern + ")(?:\\s+(?:" + tokenPattern + "))*\\s*$";
}
}

CallgrindHighlighter::CallgrindHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    // Header tokens
    descFormat.setForeground(Qt::darkMagenta);   // desc:
    cmdFormat.setForeground(Qt::darkCyan);         // cmd:
    partFormat.setForeground(Qt::darkYellow);      // part:
    eventsFormat.setForeground(Qt::blue);          // events:
    summaryFormat.setForeground(Qt::darkBlue);     // summary:

    // File and function tokens
    fileFormat.setForeground(Qt::blue);            // fl=
    functionFormat.setForeground(Qt::darkGreen);     // fn=
    cflFormat.setForeground(Qt::darkRed);            // cfl=
    cfnFormat.setForeground(Qt::magenta);            // cfn=
    cfiFormat.setForeground(Qt::darkGray);           // cfi=
    callsFormat.setForeground(Qt::darkYellow);       // calls=

    // Numeric cost lines
    costFormat.setForeground(Qt::red);

    // Comments
    commentFormat.setForeground(Qt::gray);
    commentFormat.setFontItalic(true);

    // -- Highlighting Rules ---

    // Header lines:
    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression("^desc:.*");
        rule.format = descFormat;
        highlightingRules.append(rule);
    }
    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression("^cmd:.*");
        rule.format = cmdFormat;
        highlightingRules.append(rule);
    }
    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression("^part:.*");
        rule.format = partFormat;
        highlightingRules.append(rule);
    }
    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression("^events:.*");
        rule.format = eventsFormat;
        highlightingRules.append(rule);
    }
    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression("^summary:.*");
        rule.format = summaryFormat;
        highlightingRules.append(rule);
    }

    // File and function identifiers:
    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression("^fl=.*");
        rule.format = fileFormat;
        highlightingRules.append(rule);
    }
    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression("^fn=.*");
        rule.format = functionFormat;
        highlightingRules.append(rule);
    }
    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression("^cfl=.*");
        rule.format = cflFormat;
        highlightingRules.append(rule);
    }
    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression("^cfn=.*");
        rule.format = cfnFormat;
        highlightingRules.append(rule);
    }
    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression("^cfi=.*");
        rule.format = cfiFormat;
        highlightingRules.append(rule);
    }
    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression("^calls=.*");
        rule.format = callsFormat;
        highlightingRules.append(rule);
    }

    // Numeric cost lines
    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(buildCostLinePattern());
        rule.format = costFormat;
        highlightingRules.append(rule);
    }

    // Comments
    {
        HighlightingRule rule;
        rule.pattern = QRegularExpression("^#.*");
        rule.format = commentFormat;
        highlightingRules.append(rule);
    }
}

void CallgrindHighlighter::highlightBlock(const QString &text)
{
    // Cost Lines Highlighting
    QRegularExpression costLineRegex(buildCostLinePattern());
    if (costLineRegex.match(text).hasMatch()) {
        setFormat(0, text.length(), costFormat);
        return;
    }

    // Otherwise, apply all other highlighting rules.
    for (const HighlightingRule &rule : std::as_const(highlightingRules)) {
        QRegularExpressionMatchIterator i = rule.pattern.globalMatch(text);
        while (i.hasNext()) {
            QRegularExpressionMatch match = i.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
