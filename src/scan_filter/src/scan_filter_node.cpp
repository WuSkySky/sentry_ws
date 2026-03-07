#include <memory>
#include <vector>
#include <cmath>
#include <limits>

#include "scan_filter/scan_filter_node.hpp"

ScanFilterNode::ScanFilterNode(const rclcpp::NodeOptions & options)
: rclcpp::Node("scan_filter_node", options)
{
        // 订阅原始激光扫描数据
        subscription_ = this->create_subscription<sensor_msgs::msg::LaserScan>(
            "/scan", 10,
            std::bind(&ScanFilterNode::scan_callback, this, std::placeholders::_1));

        // 发布过滤后的激光扫描数据
        publisher_ = this->create_publisher<sensor_msgs::msg::LaserScan>("/scan_filtered", 10);
}

ScanFilterNode::~ScanFilterNode()
{

}

void ScanFilterNode::scan_callback(const sensor_msgs::msg::LaserScan::SharedPtr msg)
{
    // 用于保存筛选后的点
    std::vector<float> filtered_ranges;
    std::vector<float> filtered_intensities;

    const float angle_min = msg->angle_min;
    const float angle_increment = msg->angle_increment;

    // 遍历所有激光点
    for (size_t i = 0; i < msg->ranges.size(); ++i)
    {
        // 当前激光点的角度
        float angle = angle_min + static_cast<float>(i) * angle_increment;

        // 判断角度时候在范围中
        if (angle >= -M_PI / 2.0 && angle <= M_PI / 2.0)
        {
            // 保留该点：添加到 filtered_ranges
            filtered_ranges.push_back(msg->ranges[i]);

            filtered_intensities.push_back(msg->intensities[i]);

        }
    }

    // 创建新的 LaserScan 消息
    auto filtered_scan = std::make_shared<sensor_msgs::msg::LaserScan>();

    // 拷贝基础信息
    filtered_scan->header = msg->header;
    filtered_scan->angle_min = -M_PI / 2.0;  
    filtered_scan->angle_max = M_PI / 2.0;  
    filtered_scan->angle_increment = angle_increment;
    filtered_scan->time_increment = msg->time_increment;
    filtered_scan->scan_time = msg->scan_time;
    filtered_scan->range_min = msg->range_min;
    filtered_scan->range_max = msg->range_max;

    // 设置筛选后的 ranges
    filtered_scan->ranges.assign(filtered_ranges.begin(), filtered_ranges.end());

    // 设置intensities
    filtered_scan->intensities.assign(filtered_intensities.begin(), filtered_intensities.end());

    // 发布筛选后的激光扫描
    publisher_->publish(*filtered_scan);
}


int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ScanFilterNode>());
    rclcpp::shutdown();
    return 0;
}