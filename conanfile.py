from conan import ConanFile
from conan.tools.files import copy
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout

from typing import List

import os


class Recipe(ConanFile):
    name = 'serial-plotter'
    version = '0.0.2'

    description = 'Interactive tool for plotting data received via a serial port'
    settings = 'os', 'arch', 'compiler', 'build_type'

    requires = [
        'boost/1.75.0',
        'sdl/2.0.14',
        'imgui/1.83',
        'implot/0.11',
        'glad/0.1.34'
    ]

    keep_imports = True

    def configure(self):
        # UI Tool settings
        self.options['glad'].spec = 'gl'
        self.options['glad'].gl_profile = 'core'
        self.options['glad'].gl_version = '3.2'

        # Turn off the Pulse Audio support for non-windows OSs
        if self.settings.os != 'Windows':
            self.options['sdl'].pulse = False

        if self.settings.os == 'Macos':
            self.options['boost'].with_stacktrace_backtrace = False

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def _import_bindings(self, target_dir):
        if not os.path.exists(target_dir):
            os.makedirs(target_dir)

        for dep in self.dependencies.values():
            srcdirs = dep.cpp_info.srcdirs  # type: List[str]
            if len(srcdirs) == 0:
                continue

            for src_dir in srcdirs:
                print(src_dir)
                copy(self, pattern='*.h', dst=target_dir, src=src_dir)
                copy(self, pattern='*.cpp', dst=target_dir, src=src_dir)


    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()

        cmake_deps = CMakeDeps(self)
        cmake_deps.generate()

        self._import_bindings(target_dir=os.path.join(self.build_folder, 'bindings'))

    def package(self):
        cmake = CMake(self)
        cmake.install()

