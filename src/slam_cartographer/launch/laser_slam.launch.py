from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription, ExecuteProcess
from launch.conditions import IfCondition, UnlessCondition
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node, SetRemap
from launch_ros.substitutions import FindPackageShare
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.actions import Shutdown

def generate_launch_description():

    # Cartographer 节点（订阅实时雷达）
    cartographer_node = Node(
        package='cartographer_ros',
        executable='cartographer_node',
        name='cartographer_node',
        output='screen',
        parameters=[{'use_sim_time': False}],
        remappings = [
            ('scan', 'scan')
        ],
        arguments=[
            '-configuration_directory', PathJoinSubstitution([FindPackageShare('slam_cartographer'), 'config']),
            '-configuration_basename', 'laser_slam.lua'
        ]
    )

    # 占用栅格地图发布节点
    occupancy_grid_node = Node(
        package='cartographer_ros',
        executable='cartographer_occupancy_grid_node',
        name='cartographer_occupancy_grid_node',
        output='screen',
        parameters=[{'use_sim_time': False}],
        arguments=['-resolution', '0.05', '-publish_period_sec', '1.0']
    )


    rviz_node = Node(
        package = 'rviz2',
        executable = 'rviz2',
        on_exit = Shutdown(),
        arguments = ['-d', FindPackageShare('cartographer_ros').find('cartographer_ros') + '/configuration_files/demo_2d.rviz'],
        parameters = [{'use_sim_time': False}],
    )

    return LaunchDescription([
        cartographer_node,
        occupancy_grid_node,
        rviz_node,
    ])