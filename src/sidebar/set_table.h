#pragma once

#include <QWidget>

#include "core/a_star_engine.h"
#include "core/graph.h"

class QTableWidget;
class QLabel;

class SetTable : public QWidget
{
    Q_OBJECT

public:
    explicit SetTable(Graph &graph, bool withCostColumn, const QString &emptyHint,
                      QWidget *parent = nullptr);

public slots:
    void showNodes(const QList<Graph::NodeId> &nodeIds, const AStarStep &step);
    void clearRows();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QString displayName(Graph::NodeId nodeId) const;
    void updateHint();

    Graph &m_graph;
    QTableWidget *m_table = nullptr;
    QLabel *m_hintLabel = nullptr;
};
