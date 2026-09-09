#include "mainwindow.h"

#include "board/board_toolbar.h"
#include "board/board_view.h"

#include <QFrame>
#include <QLabel>
#include <QSplitter>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Simulador A*");
    resize(1280, 800);

    auto *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setHandleWidth(1);

    m_board = new BoardView;
    m_toolbar = new BoardToolbar(m_board);
    m_board->setOverlay(m_toolbar);

    m_sidebar = new QFrame;
    m_sidebar->setObjectName("sidebar");
    m_sidebar->setMinimumWidth(280);

    auto *sidebarLayout = new QVBoxLayout(m_sidebar);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);
    sidebarLayout->setSpacing(0);

    sidebarLayout->addWidget(makeSidebarSection("Edit box"), 3);
    sidebarLayout->addWidget(makeSidebarSection("Open set"), 3);
    sidebarLayout->addWidget(makeSidebarSection("Closed set"), 3);

    splitter->addWidget(m_board);
    splitter->addWidget(m_sidebar);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({960, 320});

    setCentralWidget(splitter);
}

QWidget *MainWindow::makeSidebarSection(const QString &title) const
{
    auto *section = new QFrame;
    section->setObjectName("sidebarSection");

    auto *sectionLayout = new QVBoxLayout(section);
    sectionLayout->setContentsMargins(0, 0, 0, 0);
    sectionLayout->setSpacing(0);

    auto *titleLabel = new QLabel(title, section);
    titleLabel->setObjectName("sidebarSectionTitle");
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    auto *placeholder = new QLabel("-", section);
    placeholder->setObjectName("sectionPlaceholder");
    placeholder->setAlignment(Qt::AlignCenter);

    sectionLayout->addWidget(titleLabel);
    sectionLayout->addWidget(placeholder, 1);

    return section;
}
