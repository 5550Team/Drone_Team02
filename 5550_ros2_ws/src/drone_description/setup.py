from setuptools import find_packages, setup
from glob import glob
import os

package_name = 'drone_description'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
       ('share/' + package_name + '/urdf', ['urdf/drone_base.urdf']),
        ('share/' + package_name + '/launch', ['launch/display_rviz2.launch.py']),
        ('share/' + package_name + '/launch', ['launch/spawn_in_gazebo.launch.py']),
        ('share/' + package_name + '/worlds', ['worlds/room5.world']),
        ('share/' + package_name + '/worlds', ['worlds/room1.world']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='yahboom',
    maintainer_email='yahboom@todo.todo',
    description='TODO: Package description',
    license='TODO: License declaration',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
        ],
    },
)
