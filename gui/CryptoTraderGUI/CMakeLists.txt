cmake_minimum_required(VERSION 3.16)

project(CryptoTraderGUI VERSION 0.1 LANGUAGES CXX)

# Use Qt's auto tools
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set path to your manually built Qt
set(CMAKE_PREFIX_PATH "/opt/qt6.9.0")

# Find Qt6 modules
find_package(Qt6 REQUIRED COMPONENTS Widgets Charts)

# Find OpenSSL (used by cpp-httplib in your backend)
find_package(OpenSSL REQUIRED)

# Define source files
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

# Add the executable
add_executable(CryptoTraderGUI ${PROJECT_SOURCES})

# Include directories for your own code
target_include_directories(CryptoTraderGUI PRIVATE
    ../../include
)

# Compile definitions
target_compile_definitions(CryptoTraderGUI PRIVATE
    CPPHTTPLIB_OPENSSL_SUPPORT
)

# Link to Qt and OpenSSL
target_link_libraries(CryptoTraderGUI PRIVATE
    Qt6::Widgets
    Qt6::Charts
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Optional macOS settings (ignored on Linux)
if(APPLE)
    set_target_properties(CryptoTraderGUI PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER com.example.CryptoTraderGUI
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    )
endif()

# Install rules
include(GNUInstallDirs)
install(TARGETS CryptoTraderGUI
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)