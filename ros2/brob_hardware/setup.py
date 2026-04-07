from setuptools import find_packages, setup

package_name = 'brob_hardware'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='ingrid',
    maintainer_email='ingrid@todo.todo',
    description='TODO: Package description',
    license='Apache-2.0',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': [
     		'command_bridge = brob_hardware.command_bridge_node:main',
     		'vision_bridge = brob_hardware.vision_bridge_node:main',
        ],
    },
)
