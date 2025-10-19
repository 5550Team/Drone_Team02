import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node

def generate_launch_description():
    # 1
    drone_navigation2_dir = get_package_share_directory('drone_navigation2')
    nav2_bringup_dir = get_package_share_directory('nav2_bringup')

    # 2) 
    default_use_sim_time = 'true'  
    default_map_yaml = os.path.join(drone_navigation2_dir, 'maps', 'room5_map.yaml')
    default_params = os.path.join(drone_navigation2_dir, 'param', 'drone_nav2.yaml')
    rviz_config = os.path.join(nav2_bringup_dir, 'rviz', 'nav2_default_view.rviz')

    # 3
    use_sim_time = LaunchConfiguration('use_sim_time', default=default_use_sim_time)
    map_yaml_path = LaunchConfiguration('map', default=default_map_yaml)
    nav2_param_path = LaunchConfiguration('params_file', default=default_params)

    declare_use_sim_time = DeclareLaunchArgument(
        'use_sim_time', default_value=default_use_sim_time,
        description='Use simulation time if true'
    )
    declare_map = DeclareLaunchArgument(
        'map', default_value=default_map_yaml,
        description='Full path to map yaml to load'
    )
    declare_params = DeclareLaunchArgument(
        'params_file', default_value=default_params,
        description='Full path to the ROS2 parameters file for Nav2'
    )

    # 4) Include  bringup
    bringup = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(nav2_bringup_dir, 'launch', 'bringup_launch.py')
        ),
        launch_arguments={
            'map': map_yaml_path,
            'use_sim_time': use_sim_time,
            'params_file': nav2_param_path
        }.items()
    )

    
    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        output='screen',
        arguments=['-d', rviz_config],
        parameters=[{'use_sim_time': use_sim_time}],
    )

    return LaunchDescription([
        declare_use_sim_time,
        declare_map,
        declare_params,
        bringup,
        rviz_node
    ])

