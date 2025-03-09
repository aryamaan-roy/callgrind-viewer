# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

cmake_minimum_required(VERSION 3.16)
project(callgrindviewer LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)

if(NOT DEFINED INSTALL_EXAMPLESDIR)
    set(INSTALL_EXAMPLESDIR "examples")
endif()

set(INSTALL_EXAMPLEDIR "${INSTALL_EXAMPLESDIR}/assistant/callgrindviewer")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

qt_add_executable(callgrindviewer
    assistant.cpp assistant.h
    findfiledialog.cpp findfiledialog.h
    main.cpp
    mainwindow.cpp mainwindow.h
    textedit.cpp textedit.h
)

set_target_properties(callgrindviewer PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_compile_definitions(callgrindviewer PUBLIC
    SRCDIR="${CMAKE_CURRENT_SOURCE_DIR}/"
)

target_link_libraries(callgrindviewer PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
)

install(TARGETS callgrindviewer
    RUNTIME DESTINATION "${INSTALL_EXAMPLEDIR}"
    BUNDLE DESTINATION "${INSTALL_EXAMPLEDIR}"
    LIBRARY DESTINATION "${INSTALL_EXAMPLEDIR}"
)
