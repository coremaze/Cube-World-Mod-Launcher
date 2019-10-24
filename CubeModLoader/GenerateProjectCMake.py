from pathlib import Path
import os

path = os.path.dirname(os.path.realpath(__file__))
_, folderName = os.path.split(path)

files = list(Path(path).glob('*.cpp'))
   
for filename in files:
    print(filename)

lines = [
'cmake_minimum_required (VERSION 3.8)',
'project(project_%s)' % folderName,
'add_library (%s SHARED \n    %s)' % (folderName, '\n    '.join([str(x)[len(path)+1:].replace('\\', '/') for x in files]))
    ]

with open(os.path.join(path, 'CMakeLists.txt'), 'w') as f:
    f.write('\n'.join(lines))
