#pragma once

#include <QWidget>

#include "core/graph.h"

class BoardScene;
class QCheckBox;
class QLineEdit;
class QPushButton;

class EditBox : public QWidget
{
    Q_OBJECT

public:
    explicit EditBox(Graph &graph, BoardScene &scene, QWidget *parent = nullptr);

public slots:
    void selectNode(Graph::NodeId nodeId);
    void selectEdge(Graph::EdgeId edgeId);
    void clearSelection();

private:
    void refresh();
    QColor currentColor() const;

    void applyName();
    void applyColor();
    void applyStart(bool checked);
    void applyGoal(bool checked);

    Graph &m_graph;
    BoardScene &m_scene;
    Graph::NodeId m_nodeId = Graph::kInvalidNodeId;
    Graph::EdgeId m_edgeId = Graph::kInvalidEdgeId;
    QLineEdit *m_nameEdit = nullptr;
    QPushButton *m_colorButton = nullptr;
    QCheckBox *m_startCheck = nullptr;
    QCheckBox *m_goalCheck = nullptr;
};
