#ifndef FOLDOVERLAY_H
#define FOLDOVERLAY_H

#include <QWidget>

class FoldOverlay : public QWidget {
    Q_OBJECT
public:
    explicit FoldOverlay(QWidget *parent = nullptr);

    // number of lines folded, which will be displayed.
    void setFoldInfo(int numLines);

signals:
    // Emitted when the overlay is clicked.
    void clicked();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    int m_numLines;
};

#endif // FOLDOVERLAY_H
