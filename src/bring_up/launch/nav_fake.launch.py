from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, TimerAction
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import os


def generate_launch_description():

    ld = LaunchDescription()
    
    # 处理cmd_vel的节点
    send_node = Node(
        package='nav2_pkg',
        executable='send_fake_node',
        name='send_fake_node',
        output='screen'
    )

    ld.add_action(send_node)

    # 下位机通信
    serial_launch_file = os.path.join(
        get_package_share_directory('rm_bringup'), 
        'launch',                      
        'bringup.launch.py'                
    )

    serial_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(serial_launch_file)
    )

    ld.add_action(serial_launch)

    # 雷达
    lidar_launch_file = os.path.join(
        get_package_share_directory('rplidar_ros'), 
        'launch',                      
        'rplidar_c1_launch.py'                
    )

    lidar_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(lidar_launch_file)
    )

    ld.add_action(lidar_launch)

    # 雷达消息过滤
    scan_filter_node = Node(
        package='scan_filter',
        executable='scan_filter_node',
        name='scan_filter_node',
        output='screen'
    )

    ld.add_action(scan_filter_node)

    # # nav2_pkg导航
    # nav2_pkg_launch_file = os.path.join(
    #     get_package_share_directory('nav2_pkg'), 
    #     'launch',                      
    #     'bringup.launch.py'                
    # )

    # nav2_pkg_launch = TimerAction(
    #     period=5.0,
    #     actions=[
    #         IncludeLaunchDescription(
    #             PythonLaunchDescriptionSource(nav2_pkg_launch_file)
    #         ),
    #     ]
    # )
    # ld.add_action(nav2_pkg_launch)

    return ld