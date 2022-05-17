import os

BASE_DIR = 'D:\\Unreal Engine\\Full\\Engine\\Source\\Runtime\\Core\\Public\\Templates'

TAG_FILES = [
    ('4.0.1-release',  './Atomic.h'),
    ('4.1.0-release',  './Atomic.h'),
    ('4.2.0-release',  './Atomic.h'),
    ('4.3.0-release',  './Atomic.h'),
    ('4.4.0-release',  './Atomic.h'),
    ('4.5.0-release',  './Atomic.h'),
    ('4.6.0-release',  './Atomic.h'),
    ('4.7.0-release',  './Atomic.h'),
    ('4.8.0-release',  './Atomic.h'),
    ('4.9.0-release',  './Atomic.h'),
    ('4.10.0-release', './Atomic.h'),
    ('4.11.0-release', './Atomic.h'),
    ('4.12.0-release', './Atomic.h'),
    ('4.13.0-release', './Atomic.h'),
    ('4.14.0-release', './Atomic.h'),
    ('4.15.0-release', './Atomic.h'),
    ('4.16.0-release', './Atomic.h'),
    ('4.17.0-release', './Atomic.h'),
    ('4.18.0-release', './Atomic.h'),
    ('4.19.0-release', './Atomic.h'),
    ('4.20.0-release', './Atomic.h'),
    ('4.21.0-release', './Atomic.h'),
    ('4.22.0-release', './Atomic.h'),
    ('4.23.0-release', './Atomic.h'),
    ('4.24.0-release', './Atomic.h'),
    ('4.25.0-release', './Atomic.h'),
    ('4.26.0-release', './Atomic.h'),
    ('4.27.0-release', './Atomic.h'),
    ('5.0.0-release',  './Atomic.h')
]

if __name__ == '__main__':
    os.chdir(BASE_DIR)
    for i in range(len(TAG_FILES) - 1):
        first = f'{TAG_FILES[i][0]}:{TAG_FILES[i][1]}'
        second = f'{TAG_FILES[i + 1][0]}:{TAG_FILES[i + 1][1]}'
        print(f'{TAG_FILES[i][0]} {TAG_FILES[i + 1][0]}')
        os.system(f'git difftool {first} {second}')
