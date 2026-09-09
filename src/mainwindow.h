#pragma once

#include <QMainWindow>

#include "board/board_scene.h"
#include "core/graph.h"

class BoardToolbar;
class BoardView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QWidget *makeSidebarSection(const QString &title) const;

    Graph m_graph;
    BoardView *m_board = nullptr;
    BoardToolbar *m_toolbar = nullptr;
    BoardScene *m_scene = nullptr;
    QWidget *m_sidebar = nullptr;
};
