# Serial Plotter [![build](https://github.com/yowidin/serial-plotter/actions/workflows/main.yml/badge.svg)](https://github.com/yowidin/serial-plotter/actions/workflows/main.yml)
A bit more feature-rich replacement for the Arduino's Serial Plotter.

## About
After being annoyed by the Arduino Serial Plotters lack of functionality, and suffering from the critical NIH issue, I decided to write my own, as an exercise for the rusting C++ skills.
<br>
<img alt="showcase" height="431" src="./doc/serial-plotter-showcase.gif"/>


## Usage
The recommended way is to [download](https://github.com/yowidin/serial-plotter/releases) the pre-built binaries. Alternatively you can try to fork this project, replace the dependencies with the dependencies from the [conan repository](https://conan.io/center/) (this will require some code tweaking).

The later path will allow you to add some custom logic:
- You can add a custom data generator by providing a `data_change_cb` to the `data` object via the `set_data_change_callback` and accessing the plot data via the `data`s `get_plot_data` method. 
- You can add custom graphs to the main plot by setting the `draw_plot_cb` callback (e.g.: `ImPlot::Annotate`).

## Tech used
- [conan](https://conan.io/) with a set of [recipes](https://github.com/conan-burrito) (also suffering from the NIH Syndrome) for dependency management.
- [CMake](https://cmake.org/) for generating a build system.
- [boost](https://www.boost.org/) for async I/O and command line arguments parsing.
- [SDL2](http://www.libsdl.org) for window and event management.
- [glad](https://github.com/Dav1dde/glad) for an OpenGL loader generation.
- [imgui](https://github.com/ocornut/imgui) and [implot](https://github.com/epezent/implot) for actual UI and plot rendering.