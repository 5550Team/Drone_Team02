# Drone_Team02

## Project Overview

This repository combines an ESP32-based drone platform with a LiDAR scan mapping system to enable aerial mapping and visualization.

- `SoftwareCode`: ESP32 firmware that initializes the LiDAR serial interface, starts LiDAR scanning, and reads raw scan packets while launching the drone platform.
- `5550_ros2_ws`: ROS 2 workspace for mapping and navigation.
  - `drone_cartographer`: Cartographer launch files and a 2D SLAM configuration for LiDAR-based map building.
  - `drone_navigation2`: Navigation2 parameters for AMCL, costmaps, and path planning.
  - `drone_description`: Drone model resources, URDF, and RViz launch support.
- `SoftwareCode/Indoor_3D_Drone_Grandstation/groud_station.py`: Ground station tool that connects to the ESP32 over TCP, receives JSON telemetry, logs data, and visualizes roll, pitch, yaw, and LiDAR distance in real time.

The system is designed to publish LiDAR scan data on `/scan`, provide a fixed transform from `base_link` to `laser`, generate occupancy grid maps, and visualize mapping results in RViz.

## 项目概述

本仓库将 ESP32 无人机平台与 LiDAR 扫描建图系统融合，实现空中扫描建图与可视化。

- `SoftwareCode`：ESP32 固件，负责初始化 LiDAR 串口、启动扫描、读取雷达数据，并运行无人机平台系统。
- `5550_ros2_ws`：ROS 2 工作区，包含映射和导航模块。
  - `drone_cartographer`：Cartographer 启动文件和 2D SLAM 配置，用于 LiDAR 建图。
  - `drone_navigation2`：Nav2 参数配置，支持 AMCL、代价地图和路径规划。
  - `drone_description`：无人机模型资源、URDF 以及 RViz 可视化启动支持。
- `SoftwareCode/Indoor_3D_Drone_Grandstation/groud_station.py`：地面站工具，通过 TCP 连接 ESP32 接收 JSON 遥测数据，记录日志并实时显示姿态与雷达距离。

系统通过 `/scan` 话题传输 LiDAR 扫描数据，发布 `base_link` 到 `laser` 的静态 TF，生成占据网格地图，并在 RViz 中展示建图结果。