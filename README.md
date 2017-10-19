# WinToLinux_MultiKinectv2_ROS
A client server program, which streams the depth, body and color data to a Linux PC which runs ROS kinetic. This Program uses 4 Kinects v2 which are connected to 4 Intel NUC PCs. These NUCs are running Win 10. They are connected to a Linux PC over ethernet. The Windows PCs are collecting the data of the Kinects with the Microsoft Kinect API. The data is send via Google Protocol Buffers to the linux PC, where the messages are transformed to ROS messenges. Through the depth_proc nodelets of ROS, the depth image is transformed to a pointcloud.
# Windows Part
Visual Studio has to be installed. Here it is running version 2017. The Kinect SDK has to be installed too. To run the client program, compile on your machine and execute the file.
# Linux Part
-not uploaded yet-
