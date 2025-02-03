import shutil
import os

source_file = 'common.proto'
target_directories = [
    'bankClient/',
    'regServer/',
    'bankServer/',
    'stockClient/'
]

for directory in target_directories:
    if not os.path.exists(directory):
        os.makedirs(directory)
    shutil.copy(source_file, directory)