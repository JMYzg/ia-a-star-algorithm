#pragma once

#include <QGraphicsView>

class QWidget;

class BoardView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit BoardView(QWidget *parent = nullptr);

    void setOverlay(QWidget *overlay);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void applyZoom(qreal factor);
    void placeOverlay();

    QWidget *m_overlay = nullptr;
    bool m_panning = false;
    QPoint m_panOrigin;
    qreal m_scale = 1.0;
};
