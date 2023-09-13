# Serial Plotter [![build](https://github.com/yowidin/serial-plotter/actions/workflows/main.yml/badge.svg)](https://github.com/yowidin/serial-plotter/actions/workflows/main.yml)
A bit more feature-rich replacement for the Arduino's Serial Plotter.

## About
After being annoyed by the Arduino Serial Plotters lack of functionality, and suffering from the
critical NIH issue, I decided to write my own, as an exercise for the rusting C++ skills.
<br>
<img alt="showcase" height="431" src="./doc/serial-plotter-showcase.gif"/>


## Usage
The recommended way is to [download](https://github.com/yowidin/serial-plotter/releases) the
pre-built binaries, alternatively you can build it from sources.

The later path will allow you to add some custom logic:
- You can add a custom data generator by providing a `data_change_cb` to the `data` object via
  the `set_data_change_callback` and accessing the plot data via the `data`s `get_plot_data` method.
- You can add custom graphs to the main plot by setting the `draw_plot_cb` callback
  (e.g.: `ImPlot::Annotate`).

### Building

This project havily relies on [conan](https://conan.io/) for dependency management.


After installing conan, you will have to create a CMake cache, install dependencies and build the project:
```bash
$ git clone https://github.com/yowidin/serial-plotter.git
$ cd serial-plotter
$ mkdir cmake-build-debug
$ cd cmake-build-debug
$ conan install .. -b missing
$ conan build ..
```

## Tech used
- [conan](https://conan.io/) for dependency management.
- [CMake](https://cmake.org/) for generating a build system.
- [boost](https://www.boost.org/) for async I/O and command line arguments parsing.
- [SDL2](http://www.libsdl.org) for window and event management.
- [glad](https://github.com/Dav1dde/glad) for an OpenGL loader generation.
- [imgui](https://github.com/ocornut/imgui) and [implot](https://github.com/epezent/implot)
  for actual UI and plot rendering.
