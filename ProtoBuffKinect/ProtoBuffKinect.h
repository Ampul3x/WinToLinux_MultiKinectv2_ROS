#pragma once
//#include "addressbook.pb.h"

#define WIN32_LEAN_AND_MEAN
#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "1101"
#define _CRT_SECURE_NO_WARNINGS
#include "kinect_message.pb.h"

#include <iostream>
#include <fstream>
#include <string>
#include <io.h>
#include <Kinect.h>

#include <windows.h>
#include <windows.media.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <opencv2\core\core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\opencv.hpp>

//#include <opencv2\core.hpp>
//#include <opencv\cv.h>
//#include <opencv2/core/o>
//#include <WinSock2.h>

//#include <unistd.h>
//#include "message.pb.h"
//#include <iostream>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")



//void PromptForAddress(tutorial::Person* person);

