#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "rm_interfaces/msg/chassis_cmd.hpp"
#include "tf2_ros/transform_broadcaster.h"
#include "tf2_ros/buffer.h"
#include "tf2_ros/transform_listener.h"
#include "geometry_msgs/msg/transform_stamped.hpp"
#include "tf2/LinearMath/Quaternion.h"
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

class SendFakeNode : public rclcpp::Node
{
public:
    SendFakeNode() : Node("send_fake_node"), vyaw_(0.0)
    {
        // 创建订阅者，订阅 /cmd_vel
        cmd_vel_sub_ = this->create_subscription<geometry_msgs::msg::Twist>(
            "/cmd_vel",
            10,
            std::bind(&SendFakeNode::cmdVelCallback, this, std::placeholders::_1));

        // 创建发布者，发布自定义消息
        vehicle_control_pub_ = this->create_publisher<rm_interfaces::msg::ChassisCmd>(
            "/cmd_chassis",
            10);

        // tf 监听（改为成员变量）
        tf_buffer_ = std::make_shared<tf2_ros::Buffer>(this->get_clock());
        tf_listener_ = std::make_shared<tf2_ros::TransformListener>(*tf_buffer_);

        // tf 发布（改为成员变量）
        tf_broadcaster_ = std::make_shared<tf2_ros::TransformBroadcaster>(this);

        // timer
        timer_ = this->create_wall_timer(
            std::chrono::milliseconds(10),
            std::bind(&SendFakeNode::timerCallback, this));
    }

private:
void cmdVelCallback(const geometry_msgs::msg::Twist::SharedPtr msg)
    {
        // 获取 chassis_fake -> chassis 的变换
        geometry_msgs::msg::TransformStamped transform_stamped =
            tf_buffer_->lookupTransform("chassis", "chassis_fake", tf2::TimePointZero);

        // 手动转换 Twist：分别处理线速度和角速度向量
        geometry_msgs::msg::Twist transformed_twist;

        // --- 1. 提取旋转矩阵 ---
        const auto& rot = transform_stamped.transform.rotation;
        tf2::Quaternion q(rot.x, rot.y, rot.z, rot.w);
        tf2::Matrix3x3 rot_matrix(q);

        // --- 2. 转换线速度向量 ---
        tf2::Vector3 linear_in(msg->linear.x, msg->linear.y, msg->linear.z);
        tf2::Vector3 linear_out = rot_matrix * linear_in;
        transformed_twist.linear.x = linear_out.x();
        transformed_twist.linear.y = linear_out.y();
        transformed_twist.linear.z = linear_out.z();

        // --- 3. 转换角速度向量（伪向量，纯旋转下变换规则相同）---
        tf2::Vector3 angular_in(msg->angular.x, msg->angular.y, msg->angular.z);
        tf2::Vector3 angular_out = rot_matrix * angular_in;
        transformed_twist.angular.x = angular_out.x();
        transformed_twist.angular.y = angular_out.y();
        transformed_twist.angular.z = angular_out.z();

        // 创建自定义消息
        auto vehicle_control_msg = rm_interfaces::msg::ChassisCmd();

        // 填充 header
        vehicle_control_msg.header.stamp = this->now();
        vehicle_control_msg.header.frame_id = "chassis";

        // 填充 bool 字段
        vehicle_control_msg.is_spining = false;
        vehicle_control_msg.is_navigating = true;

        // 使用转换后的 twist
        vehicle_control_msg.twist = transformed_twist;

        // 发布
        vehicle_control_pub_->publish(vehicle_control_msg);

        // 保存角速度（转换后的）
        vyaw_ = transformed_twist.angular.z;
    }

    void timerCallback()
    {
        try
        {
            // 获取 odom -> chassis 的变换
            geometry_msgs::msg::TransformStamped ts_listening =
                tf_buffer_->lookupTransform("odom", "chassis", tf2::TimePointZero);

            // 从四元数获取当前 yaw
            tf2::Quaternion q_old(
                ts_listening.transform.rotation.x,
                ts_listening.transform.rotation.y,
                ts_listening.transform.rotation.z,
                ts_listening.transform.rotation.w);
            double yaw_old = atan2(2.0 * (q_old.w() * q_old.z() + q_old.x() * q_old.y()),
                                   1.0 - 2.0 * (q_old.y() * q_old.y() + q_old.z() * q_old.z()));

            // 计算新的 yaw
            double yaw_new_1 = yaw_old + vyaw_ * 0.01;
            double yaw_new_2 = yaw_old + vyaw_ * 0.01 - 1.57;

            // 发布第一个 TF: chassis -> chassis_fake
            geometry_msgs::msg::TransformStamped tf_send;
            tf_send.header.stamp = this->now();
            tf_send.header.frame_id = "chassis";
            tf_send.child_frame_id = "chassis_fake";
            tf_send.transform.translation.x = 0.0;
            tf_send.transform.translation.y = 0.0;
            tf_send.transform.translation.z = 0.0;
            tf2::Quaternion q_new_1;
            q_new_1.setRPY(0.0, 0.0, yaw_new_1);
            tf_send.transform.rotation.x = q_new_1.x();
            tf_send.transform.rotation.y = q_new_1.y();
            tf_send.transform.rotation.z = q_new_1.z();
            tf_send.transform.rotation.w = q_new_1.w();
            tf_broadcaster_->sendTransform(tf_send);

            // 发布第二个 TF: chassis_fake -> laser
            tf_send.header.stamp = this->now();
            tf_send.header.frame_id = "chassis_fake";
            tf_send.child_frame_id = "laser";
            tf_send.transform.translation.x = ts_listening.transform.translation.x;
            tf_send.transform.translation.y = ts_listening.transform.translation.y;
            tf_send.transform.translation.z = ts_listening.transform.translation.z;
            tf2::Quaternion q_new_2;
            q_new_2.setRPY(0.0, 0.0, yaw_new_2);
            tf_send.transform.rotation.x = q_new_2.x();
            tf_send.transform.rotation.y = q_new_2.y();
            tf_send.transform.rotation.z = q_new_2.z();
            tf_send.transform.rotation.w = q_new_2.w();
            tf_broadcaster_->sendTransform(tf_send);
        }
        catch (const tf2::TransformException &ex)
        {
            RCLCPP_WARN(this->get_logger(), "TF lookup failed: %s", ex.what());
        }
    }

    // 成员变量
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_sub_;
    rclcpp::Publisher<rm_interfaces::msg::ChassisCmd>::SharedPtr vehicle_control_pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    std::shared_ptr<tf2_ros::Buffer> tf_buffer_;
    std::shared_ptr<tf2_ros::TransformListener> tf_listener_;
    std::shared_ptr<tf2_ros::TransformBroadcaster> tf_broadcaster_;
    double vyaw_;
};

int main(int argc, char *argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<SendFakeNode>());
    rclcpp::shutdown();
    return 0;
}