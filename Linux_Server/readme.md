# Linux_Server
you have to copy these files into your catkin_ws. You have to setup ROS first. ROS kinetic is used here. When your ROS is configured well you can start the program by using one of the two launch files. The color launch file will not work, because the color data is disabled on windows side. 
# Usage
you have to name the package kinect_proto_server or edit the cmake list.
The "lurch body depth" starts the program for 4 Kinects. When you list the topics you will see 4 image_rect topics and 4 pointcloud topics. The camera_info is used to convert the depth image to points in a pointcloud. The parameters in the camera_info are taken from the iai project, which uses the Kinect v2 on Linux. You have to start the Linux program before the Windows clients.
