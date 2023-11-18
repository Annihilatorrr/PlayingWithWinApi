cmake_minimum_required(VERSION 3.5)

project(ProcessViewer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

set(TS_FILES ProcessViewer_ru_RU.ts)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        treemodel.h
        treemodel.cpp
        treeitem.h
        treeitem.cpp
        iprocessservice.h
        #iprocessservice.cpp
        processservice.h
        processservice.cpp
        #mainwindow.ui
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ProcessViewer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        fakeprocessservice.h fakeprocessservice.cpp
        processserviceexception.h

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ProcessViewer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
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

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()


include_directories(d:/projects/PlayingWithWinApi/WinApiLib)
#link_directories(d:/projects/PlayingWithWinApi/x64/Debug)

#add_library(wbemuuid.lib SHARED IMPORTED)
#add_library(Wtsapi32.lib SHARED IMPORTED)
add_library (WinApiLib SHARED IMPORTED)
set_target_properties(WinApiLib PROPERTIES
  IMPORTED_LOCATION "../WinApiLib/WinApiLib.lib"
  INTERFACE_INCLUDE_DIRECTORIES "../WinApiLib"
)
target_link_libraries(ProcessViewer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets "d:/projects/PlayingWithWinApi/x64/debug/WinApiLib.lib")

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
