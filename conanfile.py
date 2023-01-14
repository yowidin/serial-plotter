from conans import ConanFile, CMake


class Recipe(ConanFile):
    name = 'serial-plotter'
    version = '0.0.1'

    description = 'Interactive tool for plotting data received via a serial port'
    settings = 'os', 'arch', 'compiler', 'build_type'

    generators = 'cmake', 'CMakeToolchain', 'CMakeDeps'

    requires = [
        'boost/1.75.0',
        'sdl/2.0.14',
        'imgui/1.83',
        'implot/0.11',
        'glad/0.1.34'
    ]

    keep_imports = True

    def imports(self):
        self.copy('*.h', src='res/bindings/', dst='bindings')
        self.copy('*.cpp', src='res/bindings/', dst='bindings')

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def configure(self):
        self.options['glad'].spec = 'gl'
        self.options['glad'].gl_profile = 'core'
        self.options['glad'].gl_version = '3.2'
