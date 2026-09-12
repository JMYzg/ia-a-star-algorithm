#pragma once

#include <QGraphicsItem>
#include <QFont>

#include "core/graph.h"

class EdgeItem : public QGraphicsItem
{
public:
    enum { Type = UserType + 2 };

    explicit EdgeItem(Graph::EdgeId edgeId, Graph::NodeId a, Graph::NodeId b);

    int type() const override { return Type; }

    Graph::EdgeId edgeId() const;
    Graph::NodeId endpointA() const;
    Graph::NodeId endpointB() const;

    void setEndpoints(const QPointF &a, const QPointF &b);
    void setColor(const QColor &color);
    void setHighlight(bool highlighted);

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

private:
    void updateLabelGeometry();
    QPointF labelCenter() const;

    Graph::EdgeId m_edgeId;
    Graph::NodeId m_endpointA;
    Graph::NodeId m_endpointB;
    QColor m_color;
    bool m_highlighted = false;
    QPointF m_startPos;
    QPointF m_endPos;
    QLineF m_visibleLine;
    QString m_weightText;
    QFont m_labelFont;
    QRectF m_labelRect;
};
