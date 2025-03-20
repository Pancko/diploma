cmake_minimum_required(VERSION 3.16)

project(DiplomaQt VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        diploma_mainwindow.cpp
        diploma_mainwindow.h
        diploma_mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(DiplomaQt
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        cf_grammar.h cf_grammar.cpp
        automata.h automata.cpp
        regExPlus.h regExPlus.cpp
        cf_analyzer.h cf_analyzer.cpp
        cf_analyzer_session.h cf_analyzer_session.cpp
        waitingspinnerwidget.h
        waitingspinnerwidget.cpp
        QSidePanel/q_side_panel.pri
        QSidePanel/QSidePanel/math.cpp QSidePanel/QSidePanel/math.hpp QSidePanel/QSidePanel/PanelBottomSide.cpp QSidePanel/QSidePanel/PanelBottomSide.hpp QSidePanel/QSidePanel/PanelLeftSide.cpp QSidePanel/QSidePanel/PanelLeftSide.hpp QSidePanel/QSidePanel/PanelRightSide.cpp QSidePanel/QSidePanel/PanelRightSide.hpp QSidePanel/QSidePanel/PanelTopSide.cpp QSidePanel/QSidePanel/PanelTopSide.hpp QSidePanel/QSidePanel/side_panel_helpers.cpp QSidePanel/QSidePanel/side_panel_helpers.hpp QSidePanel/QSidePanel/SidePanel.cpp QSidePanel/QSidePanel/SidePanel.hpp QSidePanel/QSidePanel/SidePanelState.cpp QSidePanel/QSidePanel/SidePanelState.hpp

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET DiplomaQt APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(DiplomaQt SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(DiplomaQt
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(DiplomaQt PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.DiplomaQt)
endif()
set_target_properties(DiplomaQt PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS DiplomaQt
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(DiplomaQt)
endif()
