#!/usr/bin/env python3
import os
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.substitutions import LaunchConfiguration
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory


def generate_launch_description():
    # === 可调参数 ===
    # 1) world 路径（你已经建好的 world 文件）
    default_world = os.path.join(
        get_package_share_directory('drone_description'),
        'worlds', 'room5.world'   # ← 如果你的 world 不在这，请改成实际路径
    )
    # 2) urdf 路径（你的四旋翼 urdf）
    urdf_path = os.path.join(
        get_package_share_directory('drone_description'),
        'urdf', 'drone_base.urdf'  # ← 如果文件名不同，请改
    )

    world_arg = DeclareLaunchArgument(
        'world',
        default_value=default_world,
        description='Absolute path to world file to load'
    )

    # 读取 URDF 内容，给 robot_state_publisher 和 spawn_entity 使用
    with open(urdf_path, 'r') as f:
        robot_description = f.read()

    # 启动 Gazebo（Classic）并加载 world
    gazebo = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(
            os.path.join(
                get_package_share_directory('gazebo_ros'),
                'launch', 'gazebo.launch.py'
            )
        ),
        launch_arguments={'world': LaunchConfiguration('world')}.items()
    )

    # robot_state_publisher：发布 /robot_description，使用仿真时钟
    rsp = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        name='robot_state_publisher',
        output='screen',
        parameters=[{
            'use_sim_time': True,
            'robot_description': robot_description
        }]
    )

    # 往 Gazebo 里“生成”实体（从 /robot_description 读取）
    spawn = Node(
        package='gazebo_ros',
        executable='spawn_entity.py',
        name='spawn_quadrotor',
        output='screen',
        arguments=[
            '-entity', 'quadrotor',              # Gazebo 里的实体名
            '-topic', 'robot_description',       # 从哪个topic拉URDF
            '-x', '0', '-y', '0', '-z', '0.2'    # 初始位姿，可按需修改
        ]
    )
    
    joint_state_pub = Node(
    package='joint_state_publisher',
    executable='joint_state_publisher',
    name='joint_state_publisher',
    output='screen'
)

    ld = LaunchDescription()
    ld.add_action(world_arg)
    ld.add_action(gazebo)
    ld.add_action(rsp)
    ld.add_action(spawn)
    ld.add_action(joint_state_pub)
    return ld
