cmake_minimum_required(VERSION 2.8)
project(ros_message_user_examples)

find_package(ROS COMPONENTS catkin std_msgs sensor_msgs)
if (NOT ROS_FOUND)
  message(FATAL_ERROR "Didn't manage to find ROS components.  Did you put e.g. /opt/ros/fuerte (or wherever) in your CMAKE_PREFIX_PATH?")
endif()

add_executable(pose_multicaster pose_multicaster.cpp)
add_executable(pose_receiver    pose_receiver.cpp)




