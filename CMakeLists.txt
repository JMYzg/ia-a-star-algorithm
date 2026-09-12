cmake_minimum_required(VERSION 3.21)

project(a-star-simulator
    VERSION 0.1.0
    DESCRIPTION "Simulador interactivo del algoritmo A*"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Widgets)

qt_standard_project_setup()

qt_add_executable(a-star-simulator
    src/main.cpp
    src/mainwindow.cpp
    src/core/graph.cpp
    src/board/board_scene.cpp
    src/board/board_view.cpp
    src/board/board_toolbar.cpp
    src/board/edge_item.cpp
    src/board/node_item.cpp
    src/sidebar/edit_box.cpp
    src/app.qrc
)

target_include_directories(a-star-simulator PRIVATE src)

target_link_libraries(a-star-simulator PRIVATE Qt6::Widgets)

enable_testing()

add_executable(a-star-tests
    tests/test_a_star.cpp
    src/core/graph.cpp
    src/core/a_star_engine.cpp
)

target_include_directories(a-star-tests PRIVATE src)

target_link_libraries(a-star-tests PRIVATE Qt6::Core Qt6::Gui)

add_test(NAME a-star-tests COMMAND a-star-tests)

set_target_properties(a-star-simulator PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
