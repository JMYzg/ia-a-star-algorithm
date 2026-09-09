#pragma once

#include <QMainWindow>

class BoardView;
class BoardToolbar;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QWidget *makeSidebarSection(const QString &title) const;

    BoardView *m_board = nullptr;
    BoardToolbar *m_toolbar = nullptr;
    QWidget *m_sidebar = nullptr;
};
