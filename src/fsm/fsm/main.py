from geometry_msgs.msg import PoseStamped
from rclpy.node import Node
from nav2_simple_commander.robot_navigator import BasicNavigator
import rclpy

class FSM(Node):
    def __init__(self):
        super().__init__('FSM')

        self.state='navigate'

        self.basic_navigator = BasicNavigator()
        rclpy.spin(self.basic_navigator)
        
        self.get_logger().info('FSM 已启动')

    def run(self):
        self.get_logger().info('FSM 运行中')

        while rclpy.ok():
            if self.state == 'navigate':
                self.navigate()

    def navigate(self):
        self.get_logger().info('navigate 运行中')

        # 构造初始位姿消息
        init_pose = PoseStamped()
        init_pose.header.frame_id = 'map'
        init_pose.header.stamp = self.basic_navigator.get_clock().now().to_msg()
        init_pose.pose.position.x = 0.0
        init_pose.pose.position.y = 0.0
        init_pose.pose.position.z = 0.0
        init_pose.pose.orientation.x = 0.0
        init_pose.pose.orientation.y = 0.0
        init_pose.pose.orientation.z = 0.0
        init_pose.pose.orientation.w = 1.0

        # 发布初始位姿
        self.basic_navigator.setInitialPose(init_pose)

        self.basic_navigator.waitUntilNav2Active()

        goal_pose = PoseStamped()
        goal_pose.header.frame_id = 'map'
        goal_pose.header.stamp = self.basic_navigator.get_clock().now().to_msg()
        goal_pose.pose.position.x = 1.0
        goal_pose.pose.position.y = 2.0
        goal_pose.pose.orientation.w = 1.0  # 无旋转

        self.basic_navigator.goToPose(goal_pose)

        while not self.basic_navigator.isTaskComplete():
            pass

if __name__ == '__main__':
    rclpy.init()
    fsm_node = FSM()
    rclpy.spin(fsm_node)
    fsm_node.destroy_node()
    rclpy.shutdown()
