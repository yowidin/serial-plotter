import os
from subprocess import run

if not os.path.exists('_build'):
    os.makedirs('_build')

kwargs = {'cwd': '_build', 'check': True}
run(['conan', 'install', '..', '-b', 'missing'], **kwargs)
run(['conan', 'build', '..'], **kwargs)
