from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    command_bridge_node = Node(
        package='brob_hardware',
        executable='command_bridge',
        name='command_bridge_node',
        output='screen',
        parameters=[
            {
                'esp32_ip': '192.168.50.131',
                'esp32_port': 9001,
            }
        ]
    )

    vision_bridge_node = Node(
        package='brob_hardware',
        executable='vision_bridge',
        name='vision_bridge_node',
        output='screen',
    )

    return LaunchDescription([
        command_bridge_node,
        vision_bridge_node,
    ])
