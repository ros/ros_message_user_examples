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

      msg = ros::serialization::serializeMessage(pose);

      socket_.async_send_to(boost::asio::buffer(msg.message_start, msg.num_bytes), 
                            endpoint_,
                            boost::bind(&sender::handle_send_to, this,
                                        boost::asio::placeholders::error));
      std::cout << "    " << twirly[message_count_%4] << " " << message_count_ << "\r";
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
    if (argc != 2)
    {
      std::cerr << "Usage: sender <multicast_address>\n";
      std::cerr << "  For IPv4, try:\n";
      std::cerr << "    sender 239.255.0.1\n";
      std::cerr << "  For IPv6, try:\n";
      std::cerr << "    sender ff31::8000:1234\n";
      return 1;
    }

    boost::asio::io_service io_service;
    sender s(io_service, boost::asio::ip::address::from_string(argv[1]));
    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
