#pragma once

#include <QGraphicsObject>
#include <QColor>
#include <QString>

#include "core/graph.h"

inline constexpr qreal kNodeRadius = 20.0;

class NodeItem : public QGraphicsObject
{
    Q_OBJECT

public:
    enum class SolveVisual { None, Open, Closed, Current, Path };
    enum { Type = UserType + 1 };

    explicit NodeItem(Graph::NodeId nodeId, const QString &name, const QColor &color,
                      QGraphicsItem *parent = nullptr);

    int type() const override { return Type; }

    Graph::NodeId nodeId() const;
    void setName(const QString &name);
    void setColor(const QColor &color);
    void setRole(bool isStart, bool isGoal);
    void setSolveVisual(SolveVisual visual);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

signals:
    void positionChanged(Graph::NodeId nodeId);
    void nodeClicked(Graph::NodeId nodeId);

protected:
    QPainterPath shape() const override;
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    Graph::NodeId m_nodeId;
    QString m_name;
    QColor m_color;
    bool m_isStart = false;
    bool m_isGoal = false;
    SolveVisual m_solveVisual = SolveVisual::None;
};
