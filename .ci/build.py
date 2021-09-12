#!/usr/bin/env python3

import os
from subprocess import run

if not os.path.exists('_build'):
    os.makedirs('_build')

kwargs = {'cwd': '_build', 'check': True}
run(['conan', 'install', '..', '-b', 'missing', '-s', 'compiler.cppstd=17'], **kwargs)
run(['conan', 'build', '..'], **kwargs)
run(['cpack', '-G', 'ZIP'], **kwargs)
