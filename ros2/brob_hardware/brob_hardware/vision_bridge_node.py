import json
from urllib import request

import rclpy
from rclpy.node import Node
from std_msgs.msg import String


class VisionBridgeNode(Node):
    def __init__(self) -> None:
        super().__init__("vision_bridge_node")

        self.declare_parameter("vision_api_url", "http://127.0.0.1:8001/find")
        self.vision_api_url = self.get_parameter("vision_api_url").value

        self.publisher = self.create_publisher(String, "/vision/result", 10)
        self.subscription = self.create_subscription(
            String,
            "/vision/target",
            self.handle_target,
            10,
        )

        self.get_logger().info("vision_bridge_node ready")
        self.get_logger().info("Listening on /vision/target")
        self.get_logger().info("Publishing results to /vision/result")

    def handle_target(self, message: String) -> None:
        target = message.data.strip()
        if not target:
            return

        self.get_logger().info(f"Sending target to vision API: {target}")

        payload = json.dumps({
            "target": target,
            "save_debug_image": True,
        }).encode("utf-8")

        http_request = request.Request(
            self.vision_api_url,
            data=payload,
            headers={"Content-Type": "application/json"},
            method="POST",
        )

        try:
            with request.urlopen(http_request, timeout=60) as response:
                body = response.read().decode("utf-8")
        except Exception as error:
            result = String()
            result.data = json.dumps({
                "ok": False,
                "error": str(error),
                "target": target,
            })
            self.get_logger().error(result.data)
            self.publisher.publish(result)
            return

        result = String()
        result.data = body
        self.publisher.publish(result)
        self.get_logger().info("Published /vision/result")


def main(args=None) -> None:
    rclpy.init(args=args)
    node = VisionBridgeNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == "__main__":
    main()
