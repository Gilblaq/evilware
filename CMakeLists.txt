cmake_minimum_required(VERSION 3.21)
project(EVILWARE VERSION 3.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()
qt_add_executable(EVILWARE WIN32
  src/main.cpp
  src/mainwindow.cpp
  src/mainwindow.h
  src/terminalwidget.cpp
  src/terminalwidget.h
  src/editorwidget.cpp
  src/editorwidget.h
  src/projectwidget.cpp
  src/projectwidget.h
  src/systeminfo.cpp
  src/systeminfo.h
  src/cyberlabwidget.cpp
  src/cyberlabwidget.h
  src/pluginstorewidget.cpp
  src/pluginstorewidget.h
  src/modulepage.cpp
  src/modulepage.h
)
target_link_libraries(EVILWARE PRIVATE Qt6::Widgets)
if(WIN32)
  target_link_libraries(EVILWARE PRIVATE psapi)
endif()
install(TARGETS EVILWARE RUNTIME DESTINATION .)
