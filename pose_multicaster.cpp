//
// sender.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// tweaked to multicast ROS Poses, 2011 Troy D. Straszheim

#include <iostream>
#include <sstream>
#include <string>
#include <boost/asio.hpp>
#include "boost/bind.hpp"
#include "boost/date_time/posix_time/posix_time_types.hpp"
#include <geometry_msgs/Pose.h>
#include <ros/serialization.h>

const short multicast_port = 30001;
const int max_message_count = 1000;

const char twirly[] = "-\\|/";
class sender
{
public:
  sender(boost::asio::io_service& io_service,
      const boost::asio::ip::address& multicast_address)
    : endpoint_(multicast_address, multicast_port),
      socket_(io_service, endpoint_.protocol()),
      timer_(io_service),
      message_count_(0)
  {
    handle_timeout(boost::system::error_code());
  }

  void handle_send_to(const boost::system::error_code& error)
  {
    if (!error && message_count_ < max_message_count)
    {
      timer_.expires_from_now(boost::posix_time::milliseconds(250));
      timer_.async_wait(
          boost::bind(&sender::handle_timeout, this,
            boost::asio::placeholders::error));
    }
  }

  void handle_timeout(const boost::system::error_code& error)
  {
    if (!error)
    {
      ++message_count_;

      // put some data in the next pose
      pose.position.x = message_count_ * 1.1;
      pose.position.y = message_count_ * 2.2;
      pose.position.z = message_count_ * 3.3;
      pose.orientation.x = pose.orientation.y = pose.orientation.z = pose.orientation.w = message_count_;

      msg = ros::serialization::serializeMessage(pose);

      socket_.async_send_to(boost::asio::buffer(msg.message_start, msg.num_bytes), 
                            endpoint_,
                            boost::bind(&sender::handle_send_to, this,
                                        boost::asio::placeholders::error));
      std::cout << "    " << twirly[message_count_%4] << " " << message_count_ << " size=" << msg.num_bytes << "\r";
      std::cout.flush();
    }
  }

private:
  boost::asio::ip::udp::endpoint endpoint_;
  boost::asio::ip::udp::socket socket_;
  boost::asio::deadline_timer timer_;
  int message_count_;

  geometry_msgs::Pose pose;
  ros::SerializedMessage msg;


  std::vector<uint8_t> buf;

};

int main(int argc, char* argv[])
{
  try
  {
    const char* mcast_address = argc > 1 ? argv[1] : "224.0.0.1";
    std::cout << "Multicasting to " << mcast_address << "\n";
    boost::asio::io_service io_service;
    sender s(io_service, boost::asio::ip::address::from_string(mcast_address));
    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
