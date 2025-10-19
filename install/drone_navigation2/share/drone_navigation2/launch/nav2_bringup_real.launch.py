import os
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from ament_index_python.packages import get_package_share_directory
from launch_ros.actions import Node

def generate_launch_description():
    pkg_share = get_package_share_directory('drone_navigation2')

    use_sim_time = LaunchConfiguration('use_sim_time')
    map_yaml     = LaunchConfiguration('map')
    params_file  = LaunchConfiguration('params_file')
    scan_topic   = LaunchConfiguration('scan_topic')

    declares = [
        DeclareLaunchArgument('use_sim_time', default_value='false'),
        DeclareLaunchArgument('map',         default_value=os.path.join(pkg_share, 'maps', 'map.yaml')),
        DeclareLaunchArgument('params_file', default_value=os.path.join(pkg_share, 'param', 'drone_nav2.yaml')),
        DeclareLaunchArgument('scan_topic',  default_value='/scan'),
    ]

    # 把 AMCL / costmap 等里用到的 scan 统一重映射为 scan_topic
    nav2 = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(get_package_share_directory('nav2_bringup'), 'launch', 'bringup_launch.py')
        ),
        launch_arguments={
            'use_sim_time': use_sim_time,
            'params_file':  params_file,
            'map':          map_yaml
        }.items()
    )

    # RViz（可用官方 Nav2 默认视图）
    rviz_cfg = os.path.join(get_package_share_directory('nav2_bringup'), 'rviz', 'nav2_default_view.rviz')
    rviz = Node(package='rviz2', executable='rviz2', name='rviz2',
                arguments=['-d', rviz_cfg], output='screen')

    return LaunchDescription(declares + [nav2, rviz])
