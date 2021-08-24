from conans import ConanFile, CMake, tools

import os


class Recipe(ConanFile):
    name = 'serial-plotter'
    version = '0.0.1'

    description = 'Interactive tool for plotting data received via a serial port'
    settings = 'os', 'arch', 'compiler', 'build_type'

    generators = 'cmake'

    requires = [
        'boost/1.75.0@conan-burrito/stable',
        'SDL2/2.0.14@conan-burrito/stable',
        'imgui/1.83@conan-burrito/stable',
        'implot/0.11@conan-burrito/stable',
        'glad/0.1.34@conan-burrito/stable'
    ]

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def configure(self):
        self.options['glad'].spec = 'gl'
        self.options['glad'].gl_profile = 'core'
        if self.settings.os in ['Android', 'iOS']:
            self.options['glad'].gles2_version = '3.0'
        else:
            self.options['glad'].gl_version = '3.2'

    def imports(self):
        # Copy generated Java sources
        if self.settings.os == 'Android' and not self.options.headless:
            # Copy Java sources
            script_path = os.path.dirname(os.path.abspath(__file__))
            generated = os.path.join(script_path, 'android', 'app', 'src', 'main', 'generated')

            target = os.path.join(generated, 'java')
            source = os.path.join('share', 'java')
            if not os.path.exists(target):
                os.makedirs(target)

            self.copy('*.java', target, source)
