from launch import LaunchDescription
from launch_ros.actions import Node
from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import os

def generate_launch_description():
    config_path = os.path.join(
        get_package_share_directory('dm_imu'),
        'config',
        'ekf.yaml'
    )

    return LaunchDescription([
        Node(
            package='dm_imu',       
            executable='dm_imu_node',  # 可执行文件名
            name='dm_imu_node',     # 节点名称（可自定义）
            output='screen',        # 输出到屏幕
            parameters=[{           # 参数配置
                'port': '/dev/ttyACM0',
                'baud': 921600
            }]
        ),
    ])
