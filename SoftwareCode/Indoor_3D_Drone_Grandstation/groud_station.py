"""
Ground Station Telemetry Receiver with Real-Time Visualization
--------------------------------------------------------------

Description:
This program connects to an ESP32 drone via TCP, receives JSON telemetry data,
parses it, logs it locally, and visualizes Roll, Pitch, Yaw, and Distance in real time.

Usage:
    python ground_station_visualizer.py

Dependencies:
    pip install matplotlib
"""

import socket
import json
import time
import threading
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from collections import deque

# ---------------- Configuration ----------------
ESP32_IP = "192.168.4.1"     # IP of ESP32 (AP mode)
ESP32_PORT = 8080            # TCP Port
RECONNECT_DELAY = 3          # seconds
LOG_FILE = "telemetry_log.txt"

# ---------------- Data Buffers ----------------
MAX_POINTS = 100             # points in rolling window for plot
time_buffer = deque(maxlen=MAX_POINTS)
roll_buffer = deque(maxlen=MAX_POINTS)
pitch_buffer = deque(maxlen=MAX_POINTS)
yaw_buffer = deque(maxlen=MAX_POINTS)
distance_buffer = deque(maxlen=MAX_POINTS)

lock = threading.Lock()

# ---------------- Utility Functions ----------------
def parse_json(data_str):
    """Safely parse a JSON string."""
    try:
        return json.loads(data_str)
    except json.JSONDecodeError:
        return None


def log_packet(packet):
    """Save packet data to local log file."""
    with open(LOG_FILE, "a") as f:
        f.write(json.dumps(packet) + "\n")


def update_buffers(packet):
    """Store new values for plotting."""
    with lock:
        timestamp = time.time()
        time_buffer.append(timestamp)
        attitude = packet.get("attitude", {})
        sensors = packet.get("sensors", {})
        roll_buffer.append(attitude.get("roll", 0))
        pitch_buffer.append(attitude.get("pitch", 0))
        yaw_buffer.append(attitude.get("yaw", 0))
        distance_buffer.append(sensors.get("lidar_distance", 0))


# ---------------- Communication Thread ----------------
def receive_data():
    """Connect to ESP32 and receive telemetry data continuously."""
    while True:
        try:
            print(f"[INFO] Connecting to ESP32 at {ESP32_IP}:{ESP32_PORT} ...")
            client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client.connect((ESP32_IP, ESP32_PORT))
            print("[INFO] Connected successfully!")

            buffer = ""
            while True:
                data = client.recv(1024).decode(errors="ignore")
                if not data:
                    raise ConnectionError("Connection lost.")
                buffer += data

                # Process each JSON line
                while "\n" in buffer:
                    line, buffer = buffer.split("\n", 1)
                    packet = parse_json(line)
                    if packet:
                        log_packet(packet)
                        update_buffers(packet)

        except (ConnectionRefusedError, ConnectionError, OSError):
            print(f"[WARN] Connection failed. Retrying in {RECONNECT_DELAY}s ...")
            time.sleep(RECONNECT_DELAY)
            continue
        except KeyboardInterrupt:
            print("[INFO] Stopping data receiver.")
            break


# ---------------- Visualization ----------------
plt.style.use("seaborn-v0_8-darkgrid")
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(8, 6))
fig.suptitle("ESP32 Drone Telemetry Visualization", fontsize=14)

# Attitude plot
ax1.set_title("Attitude Angles (Roll, Pitch, Yaw)")
ax1.set_xlabel("Time (s)")
ax1.set_ylabel("Angle (°)")
roll_line, = ax1.plot([], [], label="Roll", color="tab:red")
pitch_line, = ax1.plot([], [], label="Pitch", color="tab:green")
yaw_line, = ax1.plot([], [], label="Yaw", color="tab:blue")
ax1.legend(loc="upper left")

# Distance plot
ax2.set_title("LiDAR Distance")
ax2.set_xlabel("Time (s)")
ax2.set_ylabel("Distance (m)")
dist_line, = ax2.plot([], [], label="LiDAR Distance", color="tab:purple")
ax2.legend(loc="upper left")

# Animation update function
def animate(i):
    with lock:
        if len(time_buffer) == 0:
            return roll_line, pitch_line, yaw_line, dist_line
        t = [t - time_buffer[0] for t in time_buffer]  # Normalize time
        roll_line.set_data(t, roll_buffer)
        pitch_line.set_data(t, pitch_buffer)
        yaw_line.set_data(t, yaw_buffer)
        dist_line.set_data(t, distance_buffer)
        ax1.set_xlim(0, max(1, t[-1]))
        ax1.set_ylim(min(roll_buffer + pitch_buffer + yaw_buffer) - 5,
                     max(roll_buffer + pitch_buffer + yaw_buffer) + 5)
        ax2.set_xlim(0, max(1, t[-1]))
        ax2.set_ylim(min(distance_buffer) - 0.5, max(distance_buffer) + 0.5)
        return roll_line, pitch_line, yaw_line, dist_line

ani = animation.FuncAnimation(fig, animate, interval=200, blit=False)

# ---------------- Main ----------------
if __name__ == "__main__":
    print("=== Ground Station Telemetry Receiver + Visualizer ===")
    receiver_thread = threading.Thread(target=receive_data, daemon=True)
    receiver_thread.start()
    plt.show()
