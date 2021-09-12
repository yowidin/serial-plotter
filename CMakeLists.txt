cmake_minimum_required(VERSION 3.9)
project(serial_plotter VERSION 0.0.1)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(NO_OUTPUT_DIRS)

set(ASP_WINDOWS OFF)
set(ASP_APPLE OFF)
set(ASP_UNIX OFF)

#################################################################################
### Setup platform-specific flags
if(APPLE)
    set(ASP_APPLE ON)
elseif(UNIX)
    set(ASP_UNIX ON)
elseif(WIN32)
    set(ASP_WINDOWS ON)
endif()

message(STATUS "Windows: ${ASP_WINDOWS}")
message(STATUS "Apple: ${ASP_APPLE}")
message(STATUS "Unix: ${ASP_UNIX}")

#################################################################################
### Setup dependencies
find_package(IMGUI CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(ImPlot CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS program_options)

set(ASP_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(ASP_GENERATED_INCLUDE_DIR "${ASP_GENERATED_DIR}/include")
set(ASP_GENERATED_CONFIG_HEADER "${ASP_GENERATED_INCLUDE_DIR}/asp/config.h")
configure_file(cmake/config.h.in ${ASP_GENERATED_CONFIG_HEADER} @ONLY)

if(ASP_APPLE)
    add_executable(serial-plotter MACOSX_BUNDLE)
else()
    add_executable(serial-plotter)
endif()

target_sources(serial-plotter PRIVATE
   src/main.cpp
   src/options.cpp
   src/ui/std_input_text.cpp

   src/application.cpp
   src/ui/window.cpp
   src/ui/logs.cpp

   src/inputs/serial.cpp
   src/inputs/data.cpp
)

set_target_properties(serial-plotter
   PROPERTIES
       CXX_STANDARD 17
       CXX_STANDARD_REQUIRED YES
       CXX_EXTENSIONS NO

       VERSION 0.0.1
       SOVERSION 0
)

target_link_libraries(serial-plotter PUBLIC
   ${CONAN_LIBS_SDL2} ${CONAN_LIBS_GLAD}
   imgui::opengl3 imgui::sdl implot::implot Boost::program_options
)

target_include_directories(serial-plotter SYSTEM
   PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
   PUBLIC $<BUILD_INTERFACE:${ASP_GENERATED_INCLUDE_DIR}>
)

#################################################################################
### Packaging
include(CPack)
include(GNUInstallDirs)
install(
    TARGETS serial-plotter
    BUNDLE DESTINATION ${CMAKE_INSTALL_BINDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

