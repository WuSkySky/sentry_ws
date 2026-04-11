#ifndef SCAN_FILTER_NODE_HPP_
#define SCAN_FILTER_NODE_HPP_

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"

class ScanFilterNode : public rclcpp::Node
{
public:
    explicit ScanFilterNode(const rclcpp::NodeOptions & options = rclcpp::NodeOptions());
    virtual ~ScanFilterNode();

private:
    rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr subscription_;
    rclcpp::Publisher<sensor_msgs::msg::LaserScan>::SharedPtr publisher_;

    void scan_callback(const sensor_msgs::msg::LaserScan::SharedPtr msg);
};

#endif // SCAN_FILTER_NODE_HPP_