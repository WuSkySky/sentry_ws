编译 sentry_ws中执行
```bash
colcon build
```

source
```bash
source ./install/setup.bash
```

设置ros2分组
```bash
export ROS_DOMAIN_ID=1
```

启动雷达
```bash
ros2 launch rplidar_ros rplidar_c1_launch.py
```

启动雷达及rviz2可视化
```bash
ros2 launch rplidar_ros view_rplidar_c1_launch.py
```

启动rviz2可视化
```bash
ros2 launch rplidar_ros view.launch.py
```