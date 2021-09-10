import os

if not os.path.exists('_build'):
    os.makedirs('_build')

os.chdir('_build')
os.system('conan install .. -b missing')
os.system('conan build ..')