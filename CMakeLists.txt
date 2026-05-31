cmake_minimum_required(VERSION 3.16)

project(perfin VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

qt_standard_project_setup()

add_subdirectory(core)
add_subdirectory(ui)

qt_add_executable(pilotlog
  main.cpp
)

target_link_libraries(pilotlog PRIVATE ui core Qt6::Widgets Qt6::Sql)

set_target_properties(pilotlog PROPERTIES
  WIN32_EXECUTABLE ON
  MACOSX_BUNDLE ON
)
