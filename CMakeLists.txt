cmake_minimum_required(VERSION 3.21)
project(EVILWARE VERSION 3.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Python3 REQUIRED COMPONENTS Interpreter)
execute_process(
  COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/tools/generate_assets.py ${CMAKE_CURRENT_SOURCE_DIR}/assets
  RESULT_VARIABLE EVILWARE_ASSET_RESULT
  OUTPUT_VARIABLE EVILWARE_ASSET_OUTPUT
  ERROR_VARIABLE EVILWARE_ASSET_ERROR
)
if(NOT EVILWARE_ASSET_RESULT EQUAL 0)
  message(FATAL_ERROR "EVILWARE asset generation failed: ${EVILWARE_ASSET_ERROR}")
endif()
message(STATUS "${EVILWARE_ASSET_OUTPUT}")

if(WIN32)
  enable_language(RC)
endif()

find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets Network)
qt_standard_project_setup()
set(EVILWARE_SOURCES
  src/main.cpp
  src/mainwindow.cpp
  src/mainwindow.h
  src/appportfoliowidget.cpp
  src/appportfoliowidget.h
  src/terminalwidget.cpp
  src/terminalwidget.h
  src/editorwidget.cpp
  src/editorwidget.h
  src/projectwidget.cpp
  src/projectwidget.h
  src/systeminfo.cpp
  src/systeminfo.h
  src/systemmonitorwidget.cpp
  src/systemmonitorwidget.h
  src/cyberlabwidget.cpp
  src/cyberlabwidget.h
  src/pluginstorewidget.cpp
  src/pluginstorewidget.h
  src/modulepage.cpp
  src/modulepage.h
  src/startupwidget.cpp
  src/startupwidget.h
  src/lamiaaiwidget.cpp
  src/lamiaaiwidget.h
  resources.qrc
)
if(WIN32)
  set(EVILWARE_ICON_RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/assets/evilware_app_icon.ico")
  configure_file(src/evilware.rc.in "${CMAKE_CURRENT_BINARY_DIR}/evilware.rc" @ONLY)
  list(APPEND EVILWARE_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/evilware.rc")
endif()
qt_add_executable(EVILWARE WIN32 ${EVILWARE_SOURCES})
target_link_libraries(EVILWARE PRIVATE Qt6::Widgets Qt6::Network)
if(WIN32)
  target_link_libraries(EVILWARE PRIVATE psapi crypt32)
endif()
install(TARGETS EVILWARE RUNTIME DESTINATION .)
