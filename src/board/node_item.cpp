#include "node_item.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

namespace {
constexpr qreal kBorderWidth = 2.0;
constexpr qreal kRoleRingWidth = 2.0;
constexpr qreal kStartRingGap = 3.0;
constexpr qreal kGoalRingGap = 6.0;
constexpr qreal kBoundingPad = 9.0;
const QColor kBorderColor{QStringLiteral("#353b3c")};
const QColor kTextColor{QStringLiteral("#353b3c")};
const QColor kStartRingColor{QStringLiteral("#846a6a")};
const QColor kGoalRingColor{QStringLiteral("#a2999e")};
const QColor kOpenFill{QStringLiteral("#c6c7c4")};
const QColor kClosedFill{QStringLiteral("#a2999e")};
const QColor kCurrentFill{QStringLiteral("#353b3c")};
const QColor kCurrentText{QStringLiteral("#eef0f2")};
const QColor kPathFill{QStringLiteral("#846a6a")};
const QColor kPathText{QStringLiteral("#eef0f2")};
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

void NodeItem::setRole(bool isStart, bool isGoal)
{
    if (m_isStart == isStart && m_isGoal == isGoal)
        return;
    m_isStart = isStart;
    m_isGoal = isGoal;
    update();
}

void NodeItem::setSolveVisual(SolveVisual visual)
{
    if (m_solveVisual == visual)
        return;
    m_solveVisual = visual;
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

    QColor fill = m_color;
    QColor textColor = kTextColor;
    if (m_solveVisual == SolveVisual::Open) {
        fill = kOpenFill;
    } else if (m_solveVisual == SolveVisual::Closed) {
        fill = kClosedFill;
    } else if (m_solveVisual == SolveVisual::Current) {
        fill = kCurrentFill;
        textColor = kCurrentText;
    } else if (m_solveVisual == SolveVisual::Path) {
        fill = kPathFill;
        textColor = kPathText;
    }
    painter->setBrush(fill);
    painter->drawEllipse(QPointF(0, 0), kNodeRadius, kNodeRadius);

    painter->setBrush(Qt::NoBrush);
    if (m_isStart) {
        painter->setPen(QPen(kStartRingColor, kRoleRingWidth));
        painter->drawEllipse(QPointF(0, 0), kNodeRadius + kStartRingGap,
                             kNodeRadius + kStartRingGap);
    }
    if (m_isGoal) {
        painter->setPen(QPen(kGoalRingColor, kRoleRingWidth, Qt::DashLine));
        painter->drawEllipse(QPointF(0, 0), kNodeRadius + kGoalRingGap,
                             kNodeRadius + kGoalRingGap);
    }

    QFont font = painter->font();
    font.setPointSizeF(10.0);
    font.setBold(true);
    QFontMetrics metrics(font);
    const qreal maxWidth = 2.0 * (kNodeRadius - 5.0);
    const qreal nameWidth = metrics.horizontalAdvance(m_name);
    if (nameWidth > maxWidth && nameWidth > 0)
        font.setPointSizeF(font.pointSizeF() * maxWidth / nameWidth);
    painter->setFont(font);
    painter->setPen(textColor);
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
