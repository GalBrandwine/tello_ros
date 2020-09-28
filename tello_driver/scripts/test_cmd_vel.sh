#!/usr/bin/env bash

prefix=drone1

ros2 service call ${prefix}/tello_action tello_msgs/srv/TelloAction "{cmd: 'takeoff'}"
sleep 10
ros2 topic pub --once ${prefix}/cmd_vel geometry_msgs/Twist "{linear: {x: 0.0, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: 0.2}}"
sleep 5
ros2 topic pub --once ${prefix}/cmd_vel geometry_msgs/Twist "{linear: {x: 0.0, y: 0.0, z: 0.0}, angular: {x: 0.0, y: 0.0, z: -0.2}}"
sleep 5
ros2 topic pub --once ${prefix}/cmd_vel geometry_msgs/Twist
sleep 1
ros2 service call /tello_action tello_msgs/TelloAction "{cmd: 'land'}"
