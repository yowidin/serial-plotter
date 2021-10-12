#!/usr/bin/env python3

import os
from subprocess import run
from sys import platform

if not os.path.exists('_build'):
    os.makedirs('_build')

kwargs = {'cwd': '_build', 'check': True}
install_args = ['conan', 'install', '..', '-b', 'missing', '-s', 'compiler.cppstd=17']

if platform == 'darwin':
    # Build for High Sierra
    install_args.extend(['-s', 'os.version=10.13', '-s'])

run(install_args, **kwargs)
run(['conan', 'build', '..'], **kwargs)
run(['cpack', '-G', 'ZIP'], **kwargs)
