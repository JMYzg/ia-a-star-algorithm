#include "board_view.h"

#include <QMouseEvent>
#include <QScrollBar>
#include <QWheelEvent>

namespace {
constexpr qreal kMinScale = 0.3;
constexpr qreal kMaxScale = 3.0;
constexpr qreal kZoomStep = 1.15;
constexpr int kOverlayMargin = 12;
}

BoardView::BoardView(QWidget *parent)
    : QGraphicsView(parent)
{
    auto *scene = new QGraphicsScene(this);
    scene->setSceneRect(-10000, -10000, 20000, 20000);
    setScene(scene);

    setBackgroundBrush(QColor("#eef0f2"));
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    setResizeAnchor(AnchorViewCenter);
    setDragMode(QGraphicsView::NoDrag);
    setFrameShape(QFrame::NoFrame);
}

void BoardView::setOverlay(QWidget *overlay)
{
    m_overlay = overlay;
    placeOverlay();
}

void BoardView::refreshOverlay()
{
    placeOverlay();
}

void BoardView::placeOverlay()
{
    if (!m_overlay)
        return;

    const QSize size = m_overlay->sizeHint();
    const int x = width() - size.width() - kOverlayMargin;
    m_overlay->setGeometry(x, kOverlayMargin, size.width(), size.height());
    m_overlay->raise();
}

void BoardView::setPanEnabled(bool enabled)
{
    m_panEnabled = enabled;
}

void BoardView::applyZoom(qreal factor)
{
    const qreal next = m_scale * factor;
    if (next < kMinScale || next > kMaxScale)
        return;

    m_scale = next;
    scale(factor, factor);
}

void BoardView::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() == 0) {
        QGraphicsView::wheelEvent(event);
        return;
    }

    applyZoom(event->angleDelta().y() > 0 ? kZoomStep : 1.0 / kZoomStep);
    event->accept();
}

void BoardView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton
        || (event->button() == Qt::LeftButton && m_panEnabled && !itemAt(event->pos()))) {
        m_panning = true;
        m_panOrigin = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }

    QGraphicsView::mousePressEvent(event);
}

void BoardView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_panning) {
        const QPoint delta = event->pos() - m_panOrigin;
        m_panOrigin = event->pos();
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        event->accept();
        return;
    }

    QGraphicsView::mouseMoveEvent(event);
}

void BoardView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_panning) {
        m_panning = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
        return;
    }

    QGraphicsView::mouseReleaseEvent(event);
}

void BoardView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    placeOverlay();
}
