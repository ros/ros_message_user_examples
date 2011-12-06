First prototype of "standalone" ROS messages
============================================

This small package is the first demonstration of using ROS messages as
small, light components.  There are two programs included: one that
multicasts ``geometry_msgs::Pose``\ -s on ``224.0.0.1`` (per the IANA,
this means *all hosts on the same network segment*), a second that
joins this multicast group and displays the contents of the incoming
messages.

The networking code itself are simply lightly-hacked examples from the
``boost::asio`` distribution.  The cmake invocations are enabled via
"catkin", a collection of cmake infrastructure used for building and
packaging.  This is coming soon to a ROS distribution near you.

HOW TO USE
----------

Find an i386 or amd64 machine running Ubuntu, versions Lucid
through Oneiric::

  % lsb_release -a
  No LSB modules are available.
  Distributor ID:	Ubuntu
  Description:	Ubuntu 11.04
  Release:	11.04
  Codename:	natty

Add the ppa containing the base ROS packages::

  % sudo add-apt-repository ppa:straszheim/ros
  Executing: gpg --ignore-time-conflict --no-options --no-default-keyring --secret-keyring /etc/apt/secring.gpg --trustdb-name /etc/apt/trustdb.gpg --keyring /etc/apt/trusted.gpg --primary-keyring /etc/apt/trusted.gpg --keyserver hkp://keyserver.ubuntu.com:80/ --recv 28DC864133B8140A574339405D1A022D930A190D
  gpg: requesting key 930A190D from hkp server keyserver.ubuntu.com
  gpg: key 930A190D: public key "Launchpad PPA for Troy Straszheim" imported
  gpg: Total number processed: 1
  gpg:               imported: 1  (RSA: 1)

Install them.  They'll appear in /opt/ros/fuerte::

  % sudo aptitude update
  [lots of output]
  % sudo apt-get install ros-fuerte-\*
  [ output ]
  The following NEW packages will be installed:
    ros-fuerte-catkin ros-fuerte-catkin-test-nolangs ros-fuerte-common-msgs ros-fuerte-gencpp
    ros-fuerte-genmsg ros-fuerte-genpy ros-fuerte-ros-comm ros-fuerte-roscpp-core ros-fuerte-std-msgs
  0 upgraded, 9 newly installed, 0 to remove and 55 not upgraded.
  [ more output ]

  % ls /opt/ros/fuerte 
  bin/  env.sh*  etc/  include/  lib/  setup.bash  setup.sh  setup.zsh  share/

Get the example code::

  % git clone git://github.com/ros/ros-message-user-examples.git
  Cloning into ros-message-user-examples...
  remote: Counting objects: 15, done.
  remote: Compressing objects: 100% (7/7), done.
  remote: Total 15 (delta 6), reused 15 (delta 6)
  Receiving objects: 100% (15/15), 4.35 KiB, done.
  Resolving deltas: 100% (6/6), done.
  
Run cmake on it.  Don't miss that ``CMAKE_PREFIX_PATH`` variable::

  % cd ros-message-user-examples
  % mkdir build 
  % cd build
  % cmake .. -DCMAKE_PREFIX_PATH=/opt/ros/fuerte
  -- The C compiler identification is GNU
  -- The CXX compiler identification is GNU
  -- Check for working C compiler: /home/troy/bin/gcc
  -- Check for working C compiler: /home/troy/bin/gcc -- works
  -- Detecting C compiler ABI info
  -- Detecting C compiler ABI info - done
  -- Check for working CXX compiler: /home/troy/bin/c++
  -- Check for working CXX compiler: /home/troy/bin/c++ -- works
  -- Detecting CXX compiler ABI info
  -- Detecting CXX compiler ABI info - done
  -- Boost version: 1.42.0
  -- Found the following Boost libraries:
  --   thread
  --   system
  -- Found PythonInterp: /usr/bin/python2.7 
  -- Using these generator languages from the installation: gencpp;genpy
  -- Shell environment is defined in catkin installation at /opt/ros/fuerte/env.sh
  -- Configuring done
  -- Generating done
  -- Build files have been written to: /tmp/ros-message-user-examples/build
  
Build::

  % make
  Scanning dependencies of target pose_multicaster
  [ 50%] Building CXX object CMakeFiles/pose_multicaster.dir/pose_multicaster.cpp.o
  Linking CXX executable pose_multicaster
  [ 50%] Built target pose_multicaster
  Scanning dependencies of target pose_receiver
  [100%] Building CXX object CMakeFiles/pose_receiver.dir/pose_receiver.cpp.o
  Linking CXX executable pose_receiver
  [100%] Built target pose_receiver
  
Run the "publisher", called ``pose_multicaster``.  The first
creates a ``geometry_msgs::Pose`` with (trivially) changing data
values.  By default it will send them to the ``224.0.0.1`` multicast
group::

  % ./pose_multicaster
  Multicasting to 224.0.0.1
    | 13 size=60

You'll see it counting up and spinning.  In (several) different
windows, run the ``pose_receiver``::

  % ./pose_receiver
  Joining multicast group 224.0.0.1 on iface 0.0.0.0
  position: 
    x: 45.1
    y: 90.2
    z: 135.3
  orientation: 
    x: 41
    y: 41
    z: 41
    w: 41
  
  position: 
    x: 46.2
    y: 92.4
    z: 138.6
  orientation: 
    x: 42
    y: 42
    z: 42
    w: 42
  
  [etc]


The code
--------

The code itself is a relatively minor tweak of the ``boost::asio``
multicast examples at
http://www.boost.org/doc/libs/1_48_0/doc/html/boost_asio/examples.html#boost_asio.examples.multicast.
If you compile these in release mode they are quite small::
  
  % ls -sh pose*
  220K pose_multicaster*	160K pose_receiver*

And the library dependencies are also fairly minimal::

  % ldd pose*        
  pose_multicaster:
  	linux-vdso.so.1 =>  (0x00007fffcf4af000)
  	libroscpp_serialization.so => /opt/ros/fuerte/lib/libroscpp_serialization.so (0x00007f8db1628000)
  	libboost_thread.so.1.42.0 => /usr/lib/libboost_thread.so.1.42.0 (0x00007f8db13db000)
  	libboost_system.so.1.42.0 => /usr/lib/libboost_system.so.1.42.0 (0x00007f8db11d6000)
  	libstdc++.so.6 => /usr/lib/x86_64-linux-gnu/libstdc++.so.6 (0x00007f8db0ed0000)
  	libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007f8db0c4b000)
  	libgcc_s.so.1 => /lib/x86_64-linux-gnu/libgcc_s.so.1 (0x00007f8db0a34000)
  	libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f8db06a0000)
  	libpthread.so.0 => /lib/x86_64-linux-gnu/libpthread.so.0 (0x00007f8db0482000)
  	librt.so.1 => /lib/x86_64-linux-gnu/librt.so.1 (0x00007f8db0279000)
  	/lib64/ld-linux-x86-64.so.2 (0x00007f8db182d000)
  
i.e. the boost libraries, about 100K total::

  % ls -sh /usr/lib/libboost_(thread|system).so.1.42.0
  16K /usr/lib/libboost_system.so.1.42.0	88K /usr/lib/libboost_thread.so.1.42.0
  
And ``roscpp_serialization``, also fairly light::

  % ls -sh /opt/ros/fuerte/lib/libroscpp_serialization.so 
  12K /opt/ros/fuerte/lib/libroscpp_serialization.so

``ps`` as usual, gives one a total that includes shared libraries
(that are probably mmapped into many processes)::

  % ps -FwC pose_multicaster
  UID        PID  PPID  C    SZ   RSS PSR STIME TTY          TIME CMD
  1214     23484 16959  0  5649  1492   7 15:56 pts/8    00:00:00 ./pose_multicaster

``pmap`` gives us a number (*writeable/private*) that is closer to the
incremental cost of running this program::

  % pmap -d 23484 | tail -1
  mapped: 22596K    writeable/private: 480K    shared: 0K

480k... about right.


