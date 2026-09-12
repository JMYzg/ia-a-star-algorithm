#include "edge_item.h"

#include "node_item.h"

#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOptionGraphicsItem>

namespace {
constexpr qreal kBorderWidth = 2.0;
constexpr qreal kLabelOffset = 18.0;
constexpr qreal kLabelPaddingX = 6.0;
constexpr qreal kLabelPaddingY = 3.0;
constexpr qreal kHitWidth = 12.0;
const QColor kLineColor{QStringLiteral("#353b3c")};
const QColor kLabelBackground{QStringLiteral("#eef0f2")};
const QColor kLabelTextColor{QStringLiteral("#353b3c")};
const QColor kHighlightColor{QStringLiteral("#846a6a")};
constexpr qreal kHighlightWidth = 3.0;
}

EdgeItem::EdgeItem(Graph::EdgeId edgeId, Graph::NodeId a, Graph::NodeId b)
    : m_edgeId(edgeId)
    , m_endpointA(a)
    , m_endpointB(b)
    , m_color(QColor("#353b3c"))
{
    setZValue(1.0);
    setAcceptedMouseButtons(Qt::LeftButton);
}

void EdgeItem::setColor(const QColor &color)
{
    if (m_color == color)
        return;
    m_color = color;
    update();
}

void EdgeItem::setHighlight(bool highlighted)
{
    if (m_highlighted == highlighted)
        return;
    m_highlighted = highlighted;
    update();
}

Graph::EdgeId EdgeItem::edgeId() const
{
    return m_edgeId;
}

Graph::NodeId EdgeItem::endpointA() const
{
    return m_endpointA;
}

Graph::NodeId EdgeItem::endpointB() const
{
    return m_endpointB;
}

void EdgeItem::setEndpoints(const QPointF &a, const QPointF &b)
{
    prepareGeometryChange();
    m_startPos = a;
    m_endPos = b;

    const QLineF full(a, b);
    const qreal length = full.length();
    if (length > 2.0 * kNodeRadius) {
        const QPointF unit = (b - a) / length;
        m_visibleLine = QLineF(a + unit * kNodeRadius, b - unit * kNodeRadius);
    } else {
        m_visibleLine = full;
    }

    updateLabelGeometry();
    update();
}

QPointF EdgeItem::labelCenter() const
{
    const QPointF mid = (m_startPos + m_endPos) / 2.0;
    QPointF dir = m_endPos - m_startPos;
    if (dir.isNull())
        dir = QPointF(1.0, 0.0);
    QPointF normal(dir.y(), -dir.x());
    const qreal norm = QLineF(QPointF(0, 0), normal).length();
    if (norm > 0)
        normal = normal / norm;
    return mid + normal * kLabelOffset;
}

void EdgeItem::updateLabelGeometry()
{
    m_labelFont = QFont();
    m_labelFont.setPointSizeF(9.0);
    m_labelFont.setBold(true);

    m_weightText = QString::number(QLineF(m_startPos, m_endPos).length(), 'f', 1);

    const QFontMetrics metrics(m_labelFont);
    const QSize size = metrics.size(0, m_weightText);
    m_labelRect = QRectF(QPointF(0, 0),
                         QSizeF(size.width() + 2.0 * kLabelPaddingX,
                                size.height() + 2.0 * kLabelPaddingY));
    m_labelRect.moveCenter(labelCenter());
}

QRectF EdgeItem::boundingRect() const
{
    QRectF rect = QRectF(m_visibleLine.p1(), m_visibleLine.p2()).normalized();
    rect = rect.adjusted(-kBorderWidth - 1.0, -kBorderWidth - 1.0,
                         kBorderWidth + 1.0, kBorderWidth + 1.0);
    return rect.united(m_labelRect.adjusted(-1.0, -1.0, 1.0, 1.0));
}

QPainterPath EdgeItem::shape() const
{
    QPainterPath path;
    path.moveTo(m_visibleLine.p1());
    path.lineTo(m_visibleLine.p2());
    QPainterPathStroker stroker;
    stroker.setWidth(kHitWidth);
    return stroker.createStroke(path);
}

void EdgeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing);

    painter->setPen(QPen(m_highlighted ? kHighlightColor : m_color,
                         m_highlighted ? kHighlightWidth : kBorderWidth));
    painter->drawLine(m_visibleLine);

    painter->setPen(QPen(kLineColor, 1.0));
    painter->setBrush(kLabelBackground);
    painter->drawRect(m_labelRect);

    painter->setFont(m_labelFont);
    painter->setPen(kLabelTextColor);
    painter->drawText(m_labelRect, Qt::AlignCenter, m_weightText);
}
