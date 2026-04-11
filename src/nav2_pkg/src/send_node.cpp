#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "rm_interfaces/msg/chassis_cmd.hpp"

class CmdVelRelayNode : public rclcpp::Node
{
public:
    CmdVelRelayNode() : Node("send_test_node")
    {
        // 创建订阅者，订阅 /cmd_vel
        cmd_vel_sub_ = this->create_subscription<geometry_msgs::msg::Twist>(
            "/cmd_vel",
            10,
            std::bind(&CmdVelRelayNode::cmdVelCallback, this, std::placeholders::_1));

        // 创建发布者，发布自定义消息
        vehicle_control_pub_ = this->create_publisher<rm_interfaces::msg::ChassisCmd>(
            "/cmd_chassis",
            10);
    }

private:
    void cmdVelCallback(const geometry_msgs::msg::Twist::SharedPtr msg)
    {
        // 创建自定义消息
        auto vehicle_control_msg = rm_interfaces::msg::ChassisCmd();

        // 填充 header
        vehicle_control_msg.header.stamp = this->now();
        vehicle_control_msg.header.frame_id = "test";

        // 填充 bool 字段
        vehicle_control_msg.is_spining = false;
        vehicle_control_msg.is_navigating = true;

        // 将 /cmd_vel 的 twist 值复制过来
        vehicle_control_msg.twist = *msg;

        // 发布
        vehicle_control_pub_->publish(vehicle_control_msg);
    }

    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_sub_;
    rclcpp::Publisher<rm_interfaces::msg::ChassisCmd>::SharedPtr vehicle_control_pub_;
};

int main(int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<CmdVelRelayNode>());
    rclcpp::shutdown();
    return 0;
}