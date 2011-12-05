cmake_minimum_required(VERSION 2.8)
project(ros_message_user_examples)

find_package(Boost REQUIRED COMPONENTS thread system)
find_package(ROS REQUIRED COMPONENTS catkin roscpp_serialization std_msgs sensor_msgs)
include_directories(${ROS_INCLUDE_DIRS})
include_directories(${BOOST_INCLUDE_DIRS})

if (NOT ROS_FOUND)
  message(FATAL_ERROR "Didn't manage to find ROS components.  Did you put e.g. /opt/ros/fuerte (or wherever) in your CMAKE_PREFIX_PATH?")
endif()

add_executable(pose_multicaster pose_multicaster.cpp)
target_link_libraries(pose_multicaster ${ROS_LIBRARIES} ${Boost_LIBRARIES})
add_executable(pose_receiver    pose_receiver.cpp)
target_link_libraries(pose_receiver ${ROS_LIBRARIES} ${Boost_LIBRARIES})




