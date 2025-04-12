cmake_minimum_required(VERSION 3.5)

project(CryptoTraderGUI VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt (support both Qt5 and Qt6)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Charts)

# Find OpenSSL for httplib
find_package(OpenSSL REQUIRED)

# Define project sources, including backend files
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    ../../src/DataFetcher.cpp
    ../../src/StrategyEngine.cpp
    ../../src/TradeSimulator.cpp
    ../../src/Logger.cpp
)

# Qt6-specific executable setup
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(CryptoTraderGUI
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    # Qt5 setup, with Android support
    if(ANDROID)
        add_library(CryptoTraderGUI SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(CryptoTraderGUI
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Include directories for backend headers
target_include_directories(CryptoTraderGUI PRIVATE
    ../../include
)

# Define CPPHTTPLIB_OPENSSL_SUPPORT for httplib
target_compile_definitions(CryptoTraderGUI PRIVATE
    CPPHTTPLIB_OPENSSL_SUPPORT
)

# Link libraries: Qt Widgets, Charts, and OpenSSL
target_link_libraries(CryptoTraderGUI PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Charts
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Qt-specific properties for macOS/iOS bundling
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.CryptoTraderGUI)
endif()
set_target_properties(CryptoTraderGUI PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Installation rules
include(GNUInstallDirs)
install(TARGETS CryptoTraderGUI
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Finalize Qt6 executable
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(CryptoTraderGUI)
endif()
