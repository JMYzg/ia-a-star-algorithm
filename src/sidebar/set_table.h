#pragma once

#include <QWidget>

#include "core/a_star_engine.h"
#include "core/graph.h"

class QTableWidget;

class SetTable : public QWidget
{
    Q_OBJECT

public:
    explicit SetTable(Graph &graph, bool withCostColumn, QWidget *parent = nullptr);

public slots:
    void showNodes(const QList<Graph::NodeId> &nodeIds, const AStarStep &step);
    void clearRows();

private:
    QString displayName(Graph::NodeId nodeId) const;

    Graph &m_graph;
    QTableWidget *m_table = nullptr;
};
