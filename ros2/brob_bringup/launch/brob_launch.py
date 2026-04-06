from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node


def generate_launch_description() -> LaunchDescription:
    esp32_ip_arg = DeclareLaunchArgument(
        "esp32_ip",
        default_value="190.000.0.00",
        description="ESP32 robot controller IP address",
    )
    esp32_port_arg = DeclareLaunchArgument(
        "esp32_port",
        default_value="9001",
        description="ESP32 robot controller TCP port",
    )
    vision_api_url_arg = DeclareLaunchArgument(
        "vision_api_url",
        default_value="http://127.0.0.1:8001/find",
        description="Vision API URL",
    )

    command_bridge_node = Node(
        package="brob_hardware",
        executable="command_bridge_node",
        name="command_bridge_node",
        output="screen",
        parameters=[
            {
                "esp32_ip": LaunchConfiguration("esp32_ip"),
                "esp32_port": LaunchConfiguration("esp32_port"),
            }
        ],
    )

    vision_bridge_node = Node(
        package="brob_hardware",
        executable="vision_bridge_node",
        name="vision_bridge_node",
        output="screen",
        parameters=[
            {
                "vision_api_url": LaunchConfiguration("vision_api_url"),
            }
        ],
    )

    return LaunchDescription([
        esp32_ip_arg,
        esp32_port_arg,
        vision_api_url_arg,
        command_bridge_node,
        vision_bridge_node,
    ])
