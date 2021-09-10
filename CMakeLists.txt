cmake_minimum_required(VERSION 3.9)
project(serial_plotter)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(IMGUI CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(ImPlot CONFIG REQUIRED)

set(ASP_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(ASP_GENERATED_INCLUDE_DIR "${ASP_GENERATED_DIR}/include")
set(ASP_GENERATED_CONFIG_HEADER "${ASP_GENERATED_INCLUDE_DIR}/asp/config.h")
configure_file(cmake/config.h.in ${ASP_GENERATED_CONFIG_HEADER} @ONLY)

add_executable(serial-plotter
   src/main.cpp
   src/options.cpp
   src/ui/std_input_text.cpp

   src/application.cpp
   src/ui/window.cpp
   src/ui/logs.cpp

   src/inputs/serial.cpp
   src/inputs/data.cpp
)

set_target_properties(serial-plotter PROPERTIES
   CXX_STANDARD 17
   CXX_STANDARD_REQUIRED YES
   CXX_EXTENSIONS NO
)

target_link_libraries(serial-plotter PUBLIC
   ${CONAN_LIBS}
   SDL2 imgui::opengl3 imgui::sdl implot::implot
)

target_include_directories(serial-plotter SYSTEM
   PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
   PUBLIC $<BUILD_INTERFACE:${ASP_GENERATED_INCLUDE_DIR}>
)


