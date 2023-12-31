cmake_minimum_required(VERSION 3.5)
set(CMAKE_VERBOSE_MAKEFILE on)
project(ProcessViewer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(BUILDMODE Release)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)


if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    enable_language("RC")
    set (WIN32_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/app.rc)
endif()

set(TS_FILES ProcessViewer_en_US.ts ProcessViewer_ru_RU.ts)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        processtreemodel.h
        processtreemodel.cpp
        processtablemodel.h
        processtablemodel.cpp

        processtreeitem.h
        processtreeitem.cpp
        iprocessservice.h
        processservice.h
        processservice.cpp
        #${TS_FILES}

)

set(LUPDATE_EXECUTABLE "${CMAKE_PREFIX_PATH}/bin/lupdate.exe")
set(LRELEASE_EXECUTABLE "${CMAKE_PREFIX_PATH}/bin/lrelease.exe")
message(LRELEASE_EXECUTABLE = ${LUPDATE_EXECUTABLE})
message(LUPDATE_EXECUTABLE = ${LUPDATE_EXECUTABLE})
message(CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR})
message(Running ${LUPDATE_EXECUTABLE})
add_custom_target(lupdate_target
    COMMAND ${LUPDATE_EXECUTABLE} ${PROJECT_SOURCES} -ts ${TS_FILES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running lupdate"
)

message(Resources ${PROJECT_SOURCES})
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    message("Creating executable")
    qt_add_executable(ProcessViewer
        ${QM_FILES}
        ${WIN32_RESOURCES}
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        fakeprocessservice.h fakeprocessservice.cpp
        processserviceexception.h
        processpropertiesdialog.h processpropertiesdialog.cpp
        processproperties.h
        imagetab.h imagetab.cpp
        processviewer.qrc
        processtablemodel.h processtablemodel.cpp
        processtreeviewitemdelegate.h processtreeviewitemdelegate.cpp
        processtablefiltermodel.h processtablefiltermodel.cpp
        processtreefiltermodel.h processtreefiltermodel.cpp
        filterbycomboboxmodel.h filterbycomboboxmodel.cpp
    )
    add_dependencies(ProcessViewer lupdate_target)
    # Create custom target for lrelease
    message(Running ${LRELEASE_EXECUTABLE})
    add_custom_target(lrelease_target
        COMMAND ${LRELEASE_EXECUTABLE} ${TS_FILES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Running lrelease"
    )
    message(PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")
    add_dependencies(ProcessViewer lrelease_target)
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ProcessViewer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
    message("Updating translations files")
#    add_custom_command(OUTPUT ProcessViewer_en_US.qm
#                    COMMAND lupdate
#                        -source-language en_US
#                        -target-language en_US
#                        ${PROJECT_SOURCES}
#                        -ts ProcessViewer_en_US.ts
#                    DEPENDS
#                        ${PROJECT_SOURCES}
#                    WORKING_DIRECTORY
#                        ${CMAKE_CURRENT_SOURCE_DIR}
#    )
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES} OPTIONS -source-language en_US -no-obsolete)
else()
    if(ANDROID)
        add_library(ProcessViewer SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(ProcessViewer
            ${PROJECT_SOURCES}
        )
    endif()
message("Updating translations files")
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()


include_directories(${CMAKE_SOURCE_DIR}/../WinApiLib)

#add_library(wbemuuid.lib SHARED IMPORTED)
#add_library(Wtsapi32.lib SHARED IMPORTED)
#add_library (WinApiLib SHARED IMPORTED)
#set_target_properties(WinApiLib PROPERTIES
#  IMPORTED_LOCATION "../WinApiLib/x64/release/WinApiLib.lib"
#  INTERFACE_INCLUDE_DIRECTORIES "../WinApiLib"
#)
target_link_libraries(ProcessViewer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets "${CMAKE_SOURCE_DIR}\\..\\x64\\${BUILDMODE}\\WinApiLib.lib")

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ProcessViewer)
endif()
set_target_properties(ProcessViewer PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ProcessViewer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ProcessViewer)
endif()
