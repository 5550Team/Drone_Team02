# cartographer_with_ydlidar.launch.py  (ROS 2 Humble, fixed)
import os
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription, TimerAction
from launch.substitutions import LaunchConfiguration
from launch.launch_description_sources import PythonLaunchDescriptionSource
from ament_index_python.packages import get_package_share_directory
from launch_ros.actions import Node

def generate_launch_description():
    # ---- 固定路径：你的 cartographer 配置所在包 ----
    pkg_share = get_package_share_directory('drone_cartographer')
    default_cfg_dir = os.path.join(pkg_share, 'config')  # 纯字符串

    # ---- Launch 参数（可在命令行覆盖）----
    use_sim_time = LaunchConfiguration('use_sim_time')
    scan_topic   = LaunchConfiguration('scan_topic')
    cfg_dir      = LaunchConfiguration('configuration_directory')
    cfg_base     = LaunchConfiguration('configuration_basename')

    declares = [
        DeclareLaunchArgument('use_sim_time', default_value='false'),
        DeclareLaunchArgument('scan_topic', default_value='scan'),
        DeclareLaunchArgument('configuration_directory', default_value=default_cfg_dir),
        DeclareLaunchArgument('configuration_basename',  default_value='drone_2d.lua'),
    ]

    # 1) 直接包含厂家驱动 launch（它会起驱动并发布 /scan）
    # 注意：确保此文件名确实存在于你的驱动包中（见文末“检查文件名”）
    ydlidar_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(os.path.join(
            get_package_share_directory('ydlidar_ros2_driver'),
            'launch', 'ydlidar_launch_view.py'
        ))
        # 如需强制端口/波特率，在这里加：
        # , launch_arguments={'serial_port': '/dev/ydlidar', 'serial_baudrate': '230400'}.items()
    )

    # 2) 静态 TF：base_link -> laser
    static_tf = Node(
        package='tf2_ros',
        executable='static_transform_publisher',
        name='lidar_static_tf',
        # 新式参数写法（避免旧式参数的警告）
        arguments=[
            '--x','0','--y','0','--z','0.15',
            '--roll','0','--pitch','0','--yaw','0',
            '--frame-id','base_link','--child-frame-id','laser'
        ],
        output='screen'
    )

    # 3) Cartographer（延迟启动，等驱动 & TF 就位）
    carto = Node(
        package='cartographer_ros',
        executable='cartographer_node',
        name='cartographer_node',
        output='screen',
        parameters=[{'use_sim_time': use_sim_time}],
        arguments=[
            '-configuration_directory', cfg_dir,
            '-configuration_basename',  cfg_base
        ],
        remappings=[('scan', scan_topic)]
    )
    carto_delayed = TimerAction(period=1.5, actions=[carto])

    # 4) /map 发布（占据栅格）
    occ = Node(
        package='cartographer_ros',
        executable='cartographer_occupancy_grid_node',
        name='cartographer_occupancy_grid_node',
        output='screen',
        parameters=[{'use_sim_time': use_sim_time}],
        arguments=['-resolution','0.05','-publish_period_sec','0.5']
    )

    # 5) RViz（可选）
    rviz = Node(package='rviz2', executable='rviz2', name='rviz2', output='screen')

    return LaunchDescription(declares + [ydlidar_launch, static_tf, carto_delayed, occ, rviz])

