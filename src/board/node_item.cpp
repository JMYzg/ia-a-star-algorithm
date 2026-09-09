#include "node_item.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

namespace {
constexpr qreal kNodeRadius = 20.0;
constexpr qreal kBorderWidth = 2.0;
constexpr qreal kBoundingPad = kBorderWidth + 1.0;
const QColor kBorderColor{QStringLiteral("#353b3c")};
const QColor kTextColor{QStringLiteral("#353b3c")};
}

NodeItem::NodeItem(Graph::NodeId nodeId, const QString &name, const QColor &color,
                   QGraphicsItem *parent)
    : QGraphicsObject(parent)
    , m_nodeId(nodeId)
    , m_name(name)
    , m_color(color)
{
    setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges | ItemIsFocusable);
    setZValue(10.0);
}

Graph::NodeId NodeItem::nodeId() const
{
    return m_nodeId;
}

void NodeItem::setName(const QString &name)
{
    if (m_name == name)
        return;
    m_name = name;
    update();
}

void NodeItem::setColor(const QColor &color)
{
    if (m_color == color)
        return;
    m_color = color;
    update();
}

QRectF NodeItem::boundingRect() const
{
    const qreal r = kNodeRadius + kBoundingPad;
    return QRectF(-r, -r, r * 2.0, r * 2.0);
}

QPainterPath NodeItem::shape() const
{
    QPainterPath path;
    path.addEllipse(QPointF(0, 0), kNodeRadius + kBorderWidth / 2.0,
                    kNodeRadius + kBorderWidth / 2.0);
    return path;
}

void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::Antialiasing);

    QPen pen(kBorderColor, kBorderWidth);
    if (option->state & QStyle::State_Selected) {
        pen.setColor(Qt::black);
        pen.setStyle(Qt::DashLine);
    }
    painter->setPen(pen);
    painter->setBrush(m_color);
    painter->drawEllipse(QPointF(0, 0), kNodeRadius, kNodeRadius);

    QFont font = painter->font();
    font.setPointSizeF(10.0);
    font.setBold(true);
    painter->setFont(font);
    painter->setPen(kTextColor);
    painter->drawText(boundingRect(), Qt::AlignCenter, m_name);
}

QVariant NodeItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionHasChanged)
        emit positionChanged(m_nodeId);
    return QGraphicsObject::itemChange(change, value);
}

void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit nodeClicked(m_nodeId);
    QGraphicsObject::mousePressEvent(event);
}
