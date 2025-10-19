# ROS 2 Humble — cartographer.launch.py
import os

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, SetEnvironmentVariable
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    
    color_env = SetEnvironmentVariable(name='RCUTILS_COLORIZED_OUTPUT', value='1')

    
    pkg_share = FindPackageShare(package='drone_cartographer').find('drone_cartographer')

   
    use_sim_time = LaunchConfiguration('use_sim_time', default='true') 
    resolution = LaunchConfiguration('resolution', default='0.05')
    publish_period_sec = LaunchConfiguration('publish_period_sec', default='0.5')
    configuration_directory = LaunchConfiguration(
        'configuration_directory',
        default=os.path.join(pkg_share, 'config')
    )
    configuration_basename = LaunchConfiguration(
        'configuration_basename',
        default='drone_2d.lua'
    )

    
    cartographer_node = Node(
        package='cartographer_ros',
        executable='cartographer_node',
        name='cartographer_node',
        output='screen',
        parameters=[{'use_sim_time': use_sim_time}],
        arguments=[
            '-configuration_directory', configuration_directory,
            '-configuration_basename', configuration_basename
        ],
        #remappings=[('/scan', '/lidar/lidar_ros/out')]
    )

    #
    occupancy_grid_node = Node(
        package='cartographer_ros',
        executable='cartographer_occupancy_grid_node',
        name='cartographer_occupancy_grid_node',
        output='screen',
        parameters=[{'use_sim_time': use_sim_time}],
        arguments=['-resolution', resolution, '-publish_period_sec', publish_period_sec]
    )

    # 
    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        output='screen',
        parameters=[{'use_sim_time': use_sim_time}],
       
    )

    return LaunchDescription([
        color_env,
        DeclareLaunchArgument('use_sim_time', default_value='false'),
        DeclareLaunchArgument('resolution', default_value='0.05'),
        DeclareLaunchArgument('publish_period_sec', default_value='0.5'),
        DeclareLaunchArgument('configuration_directory', default_value=os.path.join(pkg_share, 'config')),
        DeclareLaunchArgument('configuration_basename', default_value='drone_2d.lua'),
        cartographer_node,
        occupancy_grid_node,
        rviz_node
    ])

