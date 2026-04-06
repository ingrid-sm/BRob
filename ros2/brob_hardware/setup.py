from setuptools import find_packages, setup


package_name = "brob_hardware"


setup(
    name=package_name,
    version="0.0.0",
    packages=find_packages(exclude=["test"]),
    data_files=[
        ("share/ament_index/resource_index/packages", [f"resource/{package_name}"]),
        (f"share/{package_name}", ["package.xml"]),
    ],
    install_requires=["setuptools"],
    zip_safe=True,
    maintainer="Ingrid",
    maintainer_email="your_email@example.com",
    description="ROS 2 bridge nodes for the BRob robot hardware and vision API.",
    license="MIT",
    tests_require=["pytest"],
    entry_points={
        "console_scripts": [
            "command_bridge_node = brob_hardware.command_bridge_node:main",
            "vision_bridge_node = brob_hardware.vision_bridge_node:main",
        ],
    },
)
