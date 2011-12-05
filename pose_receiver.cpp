//
// receiver.cpp
// ~~~~~~~~~~~~
//
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

// tweaked to receive ROS Poses, 2011 Troy D. Straszheim

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "boost/bind.hpp"

#include <geometry_msgs/Pose.h>
#include <ros/serialization.h>

const short multicast_port = 30001;

class receiver
{
public:
  receiver(boost::asio::io_service& io_service,
      const boost::asio::ip::address& listen_address,
      const boost::asio::ip::address& multicast_address)
    : socket_(io_service)
  {
    msg.buf.reset(new uint8_t[max_length]);

    // Create the socket so that multiple may be bound to the same address.
    boost::asio::ip::udp::endpoint listen_endpoint(
        listen_address, multicast_port);
    socket_.open(listen_endpoint.protocol());
    socket_.set_option(boost::asio::ip::udp::socket::reuse_address(true));
    socket_.bind(listen_endpoint);

    // Join the multicast group.
    socket_.set_option(boost::asio::ip::multicast::join_group(multicast_address));

    socket_.async_receive_from(boost::asio::buffer(msg.buf.get(), max_length),
                               sender_endpoint_,
                               boost::bind(&receiver::handle_receive_from, this,
                                           boost::asio::placeholders::error,
                                           boost::asio::placeholders::bytes_transferred));
  }

  void handle_receive_from(const boost::system::error_code& error,
                           size_t bytes_recvd)
  {
    if (!error)
    {
      msg.num_bytes = bytes_recvd;
      msg.message_start = msg.buf.get();
      ros::serialization::deserializeMessage(msg, pose);
      ros::message_operations::Printer<geometry_msgs::Pose>::stream(std::cout, "", pose);
      std::cout << "\n";
      socket_.async_receive_from(boost::asio::buffer(msg.buf.get(), max_length), sender_endpoint_,
                                 boost::bind(&receiver::handle_receive_from, this,
                                             boost::asio::placeholders::error,
                                             boost::asio::placeholders::bytes_transferred));
    }
  }

private:
  boost::asio::ip::udp::socket socket_;
  boost::asio::ip::udp::endpoint sender_endpoint_;
  ros::SerializedMessage msg;
  geometry_msgs::Pose pose;

  enum { max_length = 1024 };
};

int main(int argc, char* argv[])
{
  try
  {
    const char* listen_addr = argc > 1 ? argv[1] : "0.0.0.0";
    const char* mcast_group = argc > 2 ? argv[2] : "224.0.0.1";

    std::cout << "Joining multicast group " << mcast_group << " on iface " << listen_addr << "\n";

    boost::asio::io_service io_service;
    receiver r(io_service,
               boost::asio::ip::address::from_string(listen_addr),
               boost::asio::ip::address::from_string(mcast_group));
    io_service.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
