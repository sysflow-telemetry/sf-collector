import os

from setuptools import setup

setup(
    name = 'SysFlow Processing API',
    version = '0.1',    
    description = ('Install SysFlow python API and utilities'),    
    packages=['sysflow'],
    package_data={'sysflow': ['schema.avsc']},
    install_requires=['avro-python3==1.8.2', 'avro-gen==0.3.0'],
    scripts=['utils/sysprint'],
    package_dir = {'': 'classes'}
)
