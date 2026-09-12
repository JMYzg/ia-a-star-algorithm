#pragma once

#include <QMainWindow>

#include "board/board_scene.h"
#include "core/graph.h"

class BoardToolbar;
class BoardView;
class EditBox;
class SetTable;
class SolveProgressBar;
class StepPlayer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QWidget *makeSidebarSection(const QString &title, QWidget *content = nullptr) const;

    Graph m_graph;
    BoardView *m_board = nullptr;
    BoardToolbar *m_toolbar = nullptr;
    BoardScene *m_scene = nullptr;
    QWidget *m_sidebar = nullptr;
    EditBox *m_editBox = nullptr;
    SetTable *m_openTable = nullptr;
    SetTable *m_closedTable = nullptr;
    SolveProgressBar *m_progressBar = nullptr;
    StepPlayer *m_player = nullptr;
};
