import socket

import rclpy
from rclpy.node import Node
from std_msgs.msg import Float32, Int32, String


class CommandBridgeNode(Node):
    def __init__(self) -> None:
        super().__init__("command_bridge_node")

        self.declare_parameter("esp32_ip", "192.168.0.10")
        self.declare_parameter("esp32_port", 9001)

        self.esp32_ip = self.get_parameter("esp32_ip").value
        self.esp32_port = self.get_parameter("esp32_port").value

        self.sock = None
        self.recv_buffer = ""

        self.subscription = self.create_subscription(
            String,
            "/brob_command",
            self.command_callback,
            10,
        )

        self.left_encoder_pub = self.create_publisher(Int32, "/left_encoder", 10)
        self.right_encoder_pub = self.create_publisher(Int32, "/right_encoder", 10)
        self.distance_pub = self.create_publisher(Float32, "/distance", 10)

        self.connect_to_esp32()

        self.timer = self.create_timer(0.05, self.read_from_esp32)

        self.get_logger().info("Command bridge node started")

    def connect_to_esp32(self) -> None:
        try:
            self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.sock.connect((self.esp32_ip, self.esp32_port))
            self.sock.setblocking(False)
            self.get_logger().info(f"Connected to ESP32 at {self.esp32_ip}:{self.esp32_port}")
        except Exception as error:
            self.get_logger().error(f"Could not connect to ESP32: {error}")
            self.sock = None

    def command_callback(self, msg: String) -> None:
        command = msg.data.strip()
        self.get_logger().info(f"Received command: {command}")

        if self.sock is None:
            self.get_logger().warn("Not connected to ESP32")
            return

        try:
            self.sock.sendall((command + "\n").encode())
            self.get_logger().info(f"Sent to ESP32: {command}")
        except Exception as error:
            self.get_logger().error(f"Failed to send command: {error}")
            self.sock = None

    def read_from_esp32(self) -> None:
        if self.sock is None:
            return

        try:
            data = self.sock.recv(1024)
            if not data:
                return

            self.recv_buffer += data.decode()

            while "\n" in self.recv_buffer:
                line, self.recv_buffer = self.recv_buffer.split("\n", 1)
                line = line.strip()

                if line.startswith("ENC,"):
                    parts = line.split(",")
                    if len(parts) == 3:
                        left_ticks = int(parts[1])
                        right_ticks = int(parts[2])

                        left_msg = Int32()
                        left_msg.data = left_ticks

                        right_msg = Int32()
                        right_msg.data = right_ticks

                        self.left_encoder_pub.publish(left_msg)
                        self.right_encoder_pub.publish(right_msg)

                elif line.startswith("DIST,"):
                    parts = line.split(",")
                    if len(parts) == 2:
                        distance_value = float(parts[1])

                        distance_msg = Float32()
                        distance_msg.data = distance_value

                        self.distance_pub.publish(distance_msg)

        except BlockingIOError:
            pass
        except Exception as error:
            self.get_logger().error(f"Error reading from ESP32: {error}")

    def destroy_node(self) -> bool:
        if self.sock is not None:
            self.sock.close()
        return super().destroy_node()


def main(args=None) -> None:
    rclpy.init(args=args)
    node = CommandBridgeNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == "__main__":
    main()
