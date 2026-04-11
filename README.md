### 环境
安装串口驱动库serial
```bash
cd serial
mkdir build
cd build

cmake ..
make
sudo make install
```

安装slam_tool_box功能包
```bash
sudo apt install ros-humble-slam-toolbox
```

安装导航功能包
```bash
sudo apt install ros-$ROS_DISTRO-navigation2
sudo apt install ros-$ROS_DISTRO-nav2-bringup
```

### build
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

### launch
启动导航
```bash
ros2 launch bring_up nav.launch.py
```

启动slam
```bash
ros2 launch bring_up slam.launch.py
```

保存地图
```bash
ros2 run nav2_map_server map_saver_cli -f room
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


### 协议
下位机至上位机

| 偏移 | 字长 | 含义     |
| ---- | ---- | -------- |
| 0    | 1    | 帧头     |
| 1    | 4    | pitch    |
| 5    | 4    | yaw      |
| 9    | 4    | roll     |
| 13   | 4    | 底盘角度 |
| 17   | 4    | x        |
| 21   | 4    | y        |
| 22   | 9    | 保留     |
| 31   | 1    | 帧尾     |

### 测试
发送cmd_el
```bash
ros2 topic pub /cmd_vel geometry_msgs/msg/Twist "{
  linear: {x: 0.0, y: 0.0, z: 0.0},
  angular: {x: 0.0, y: 0.0, z: 1.57}
}" --rate 10
```