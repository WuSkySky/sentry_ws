from launch import LaunchDescription
from launch_ros.actions import Node
from launch.substitutions import PathJoinSubstitution
from launch_ros.substitutions import FindPackageShare

def generate_launch_description():

    return LaunchDescription([
        # IMU节点
        Node(
            package='dm_imu',
            executable='dm_imu_node',
            name='dm_imu_node',
            output='screen',
            parameters=[{
                'port': '/dev/ttyACM0',
                'baud': 921600
            }]
        ),

        # RVIZ2节点
        Node(
            package='rviz2',
            executable='rviz2',
            name='rviz',
            arguments=['-d', PathJoinSubstitution([
                FindPackageShare('dm_imu'),
                'rviz',
                'dm_imu.rviz'
            ])],
            output='screen'
        )
    ])