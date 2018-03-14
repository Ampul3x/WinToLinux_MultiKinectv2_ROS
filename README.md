# WinToLinux_MultiKinectv2_ROS, Use the Data of multiple Kinect v2 on a ROS System
A client server program, which streams the depth, body and color data to a Linux PC which runs ROS kinetic. This Program uses 4 Kinects v2 which are connected to 4 Intel NUC PCs. These NUCs are running Win 10. They are connected to a Linux PC over ethernet. The Windows PCs are collecting the data of the Kinects with the Microsoft Kinect API. The data is send via Google Protocol Buffers to the linux PC, where the messages are transformed to ROS messenges. Through the depth_proc nodelets of ROS, the depth image is transformed to a pointcloud.
# Windows Part
Visual Studio has to be installed. Here, it is running version 2017. The Kinect SDK has to be installed too. To run the client program, compile on your machine and execute the file. There has to be a config.txt in the folder where you launch the .exe with the Kinectnumber and the IP Adress of the Linux Machine.
# Linux Part
ROS has to be installed. Here, kinetic is used.
# Proto.zip
This is an executable for the windows PC. Visual Studio (2015/17) and the Kinect 2 SDK has to be installed.
