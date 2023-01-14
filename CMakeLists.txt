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
find_package(glad CONFIG REQUIRED)
find_package(Boost CONFIG REQUIRED COMPONENTS program_options)

set(ASP_GENERATED_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(ASP_GENERATED_INCLUDE_DIR "${ASP_GENERATED_DIR}/include")
set(ASP_GENERATED_CONFIG_HEADER "${ASP_GENERATED_INCLUDE_DIR}/asp/config.h")
configure_file(cmake/config.h.in ${ASP_GENERATED_CONFIG_HEADER} @ONLY)

if(ASP_WINDOWS)
    # Use main() as entry point
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif()

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

   src/render/backend.cpp
   src/render/frontend.cpp
   src/render/opengl2.cpp
   src/render/opengl3.cpp
   src/render/gles2.cpp
   src/render/gles3.cpp
   src/render/detail/imgui_impl_sdl_es2.cpp
   src/render/detail/imgui_impl_sdl_es3.cpp

   ${CMAKE_CURRENT_BINARY_DIR}/bindings/imgui_impl_opengl2.cpp
   ${CMAKE_CURRENT_BINARY_DIR}/bindings/imgui_impl_opengl3.cpp
   ${CMAKE_CURRENT_BINARY_DIR}/bindings/imgui_impl_sdl.cpp
)

set_target_properties(serial-plotter
   PROPERTIES
       CXX_STANDARD 17
       CXX_STANDARD_REQUIRED YES
       CXX_EXTENSIONS NO

       VERSION 0.0.1
       SOVERSION 0
)

find_package(OpenGL REQUIRED)
target_link_libraries(serial-plotter PUBLIC
   ${CONAN_LIBS}
   imgui::imgui implot::implot Boost::program_options
   OpenGL::GL SDL2::SDL2-static SDL2::SDL2main glad::glad
)

target_include_directories(serial-plotter SYSTEM
   PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
   PUBLIC $<BUILD_INTERFACE:${ASP_GENERATED_INCLUDE_DIR}>
   PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/bindings>
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

