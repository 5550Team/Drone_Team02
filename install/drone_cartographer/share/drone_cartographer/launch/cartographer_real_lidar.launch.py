import os
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription, TimerAction
from launch.substitutions import LaunchConfiguration
from launch.launch_description_sources import PythonLaunchDescriptionSource
from ament_index_python.packages import get_package_share_directory
from launch_ros.actions import Node

def generate_launch_description():
    use_sim_time = LaunchConfiguration('use_sim_time', default='false')
    scan_topic  = LaunchConfiguration('scan_topic',  default='scan')  # 厂家默认就是 scan
    cfg_dir     = LaunchConfiguration('configuration_directory', default=os.path.join(
                        get_package_share_directory('drone_cartographer'), 'config'))
    cfg_base    = LaunchConfiguration('configuration_basename',  default='drone_2d.lua')

    # 1) 直接包含厂家驱动 launch（它会起驱动并发布 /scan）
    ydlidar_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(os.path.join(
            get_package_share_directory('ydlidar_ros2_driver'), 'launch', 'ydlidar_launch_view.py')),
        # 如需覆盖端口/波特率，可在这里加 launch_arguments
        # launch_arguments={'serial_port': '/dev/ydlidar', 'serial_baudrate': '230400'}.items(),
    )

    # 2) 静态 TF
    static_tf = Node(
        package='tf2_ros', executable='static_transform_publisher', name='lidar_static_tf',
        arguments=['0','0','0.15', '0','0','0', 'base_link','laser'],
        output='screen'
    )

    # 3) Cartographer（延迟启动，等驱动 & TF 就位）
    carto = Node(
        package='cartographer_ros', executable='cartographer_node', name='cartographer_node',
        parameters=[{'use_sim_time': use_sim_time}],
        arguments=['-configuration_directory', cfg_dir, '-configuration_basename', cfg_base],
        remappings=[('scan', scan_topic)], output='screen'
    )
    carto_delayed = TimerAction(period=1.5, actions=[carto])

    # 4) /map 发布
    occ = Node(
        package='cartographer_ros', executable='cartographer_occupancy_grid_node',
        name='cartographer_occupancy_grid_node', parameters=[{'use_sim_time': use_sim_time}],
        arguments=['-resolution','0.05','-publish_period_sec','0.5'], output='screen'
    )

    # 5) RViz
    rviz = Node(package='rviz2', executable='rviz2', name='rviz2', output='screen')

    return LaunchDescription([
        DeclareLaunchArgument('use_sim_time', default_value='false'),
        DeclareLaunchArgument('scan_topic', default_value='scan'),
        DeclareLaunchArgument('configuration_directory', default_value=str(cfg_dir.perform(None)) if hasattr(cfg_dir, 'perform') else ''),
        DeclareLaunchArgument('configuration_basename', default_value='drone_2d.lua'),
        ydlidar_launch, static_tf, carto_delayed, occ, rviz
    ])

