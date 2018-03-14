// ProtoBuffKinect.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include "ProtoBuffKinect.h"



using namespace std;
using namespace google::protobuf::io;

template<class Interface>
static inline void safeRelease(Interface *&interfaceToRelease)
{
	if (interfaceToRelease != nullptr) {
		interfaceToRelease->Release();
		interfaceToRelease = nullptr;
	}
}

int main(int argc, char* argv[])
{
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	INT16 kinectCount;
	char* host_name = "";
	string host_name_str;
	cout << "KINECT V2 Streaming Client" << endl;
	cout << "---------------------------------" << endl;
	std::ifstream configLoad;
	configLoad.open("config.txt", std::ifstream::in);
	std::cout << "Open Config File" << std::endl;
	Sleep(1500);
	for (int i = 0; i < 17; i++) {
		cout << ".";
		Sleep(50);
	}
	cout << endl;
	Sleep(1500);
	std::string line;
	std::getline(configLoad, line);
	std::cout << "kienct Num: " << line << std::endl;
	kinectCount = atoi(line.c_str());
	//kinectCount = 2;
	std::getline(configLoad, line);
	std::cout << "Server Adr: " << line << std::endl;

	host_name_str = line;
	//host_name_str = "192.168.3.1";
	std::cout << "This is Kinect " + to_string(kinectCount) << std::endl;
	host_name = strcpy((char*)malloc(host_name_str.length() + 1), host_name_str.c_str());
	cv::Mat image;

	//Kinect
	IKinectSensor *sensor = nullptr;
	IMultiSourceFrameReader *multiFrameReader = nullptr;
	byte* colorArray = new byte[1920 * 1080 * 4];
	byte* colorArrayMap = new byte[512 * 424 * 4];
	UINT size = 217088;
	UINT16 *buffer = new UINT16[217088];

	IMultiSourceFrame *multiSourceFrame = nullptr;
	ICoordinateMapper *coordinateMapper = nullptr;

	HRESULT hmap, hr, hi, hm, hc, hs, hms = GetDefaultKinectSensor(&sensor);



	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;
	cout << host_name << "  is the ip adress" << endl;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}
	else {
		cout << "WSAStartup completed" << endl;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(host_name, DEFAULT_PORT, &hints, &result);

	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}
	else {
		cout << "getaddrinfo completed" << endl;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}
		else {
			cout << "socket connection completed" << endl;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		else {
			cout << "connected to socked" << endl;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		printf("Unable to connect to server!\n");
		Sleep(1000);
		WSACleanup();
		return 1;
	}




	if (SUCCEEDED(hms)) {
		sensor->Open();
		hr = hms;
		hs = hms;
		hc = hms;
		hi = hms;
		hm = hms;
		hmap = hms;
	}




	hms = sensor->OpenMultiSourceFrameReader(FrameSourceTypes_Body |
		FrameSourceTypes_Depth |
		FrameSourceTypes_Color
		, &multiFrameReader);

	//safeRelease(sensor);
	if (FAILED(hms) || FAILED(hr) || FAILED(hs) || FAILED(hc) || FAILED(hi)) {
		Sleep(1000);
		return E_FAIL;
	}


	hm = sensor->get_CoordinateMapper(&coordinateMapper);
	while ((multiFrameReader != nullptr)) {

		IMultiSourceFrame *multiFrame = nullptr;
		hms = multiFrameReader->AcquireLatestFrame(&multiFrame);
		
		//this happens when a new frame is not available yet
		if (FAILED(hms)) {
			continue;
		}

		log_packet payload;
		
		//for ROS time stamps
		SYSTEMTIME time;
		FILETIME ft_now;
		GetSystemTime(&time);
		SystemTimeToFileTime(&time, &ft_now);
		LONGLONG ll_now = (LONGLONG)ft_now.dwLowDateTime + ((LONGLONG)(ft_now.dwHighDateTime) << 32LL);
		int seconds = ll_now / 10000000;
		int nanoseconds = (ll_now % 10000000) * 100;

		IBodyFrame *bodyFrame = nullptr;
		IDepthFrame *depthFrame = nullptr;
		IColorFrame *colorFrame = nullptr;
		IInfraredFrame *infraFrame = nullptr;


		IColorFrameReference *colorFrameRef = nullptr;
		IBodyFrameReference *bodyFrameRef = nullptr;
		IDepthFrameReference *depthFrameRef = nullptr;


		IBody *bodies[BODY_COUNT] = { 0 };
		BOOLEAN isTracked = false;
		UINT16* depthArray = new UINT16[217088];
		UINT16* infraArray = new UINT16[217088];
		ColorSpacePoint* colorSpacePoints = new ColorSpacePoint[217088];


		hc = multiFrame->get_ColorFrameReference(&colorFrameRef);
		if (SUCCEEDED(hc)) {
			hc = colorFrameRef->AcquireFrame(&colorFrame);
		}
		hr = multiFrame->get_BodyFrameReference(&bodyFrameRef);

		if (SUCCEEDED(hr)) {
			hr = bodyFrameRef->AcquireFrame(&bodyFrame);
		}
		hs = multiFrame->get_DepthFrameReference(&depthFrameRef);

		if (SUCCEEDED(hs)) {
			hs = depthFrameRef->AcquireFrame(&depthFrame);
		}

		if (SUCCEEDED(hc)) {
			hc = colorFrame->CopyConvertedFrameDataToArray(1920 * 1080 * 4, colorArray, ColorImageFormat_Bgra);
		}

		else {
			string help(217088 * 4, 'c');
			cout << "-color acquisition failed for this frame-" << endl;
			payload.add_color(help);
		}

		if (SUCCEEDED(hs)) {
			hs = depthFrame->CopyFrameDataToArray(size, depthArray);

		}

		//in the bodyVector posNOr, the timestamps and the kinectcounts are saved.
		vector<double> posNOr;
		posNOr.push_back(kinectCount);
		posNOr.push_back(seconds);
		posNOr.push_back(nanoseconds);

		if (SUCCEEDED(hr)) {
			do {
				hr = bodyFrame->GetAndRefreshBodyData(_countof(bodies), bodies);

			} while (FAILED(hr));
		}

		//body computation
		if (SUCCEEDED(hr)) {
			for (int bodyIndex = 0; bodyIndex < BODY_COUNT; bodyIndex++) {
				IBody *body = bodies[bodyIndex];
				HRESULT hr = body->get_IsTracked(&isTracked);
				
				if (FAILED(hr) || isTracked == false) {
					continue;
				}
				Joint joints[JointType_Count];
				JointOrientation jointOrientations[JointType_Count];
				hr = body->GetJoints(_countof(joints), joints);
				hr = body->GetJointOrientations(_countof(joints), jointOrientations);

				for (int i = 0; i < _countof(joints); i++) {
					posNOr.push_back(-joints[i].Position.X);
					posNOr.push_back(-joints[i].Position.Y);
					posNOr.push_back(joints[i].Position.Z);
					posNOr.push_back(jointOrientations[i].Orientation.w);
					posNOr.push_back(jointOrientations[i].Orientation.x);
					posNOr.push_back(jointOrientations[i].Orientation.y);
					posNOr.push_back(jointOrientations[i].Orientation.z);
					posNOr.push_back(joints[i].TrackingState);
				}
			}
		}


		posNOr.push_back(INFINITY);
		
		//mapping the depth frame to color frame
		if (SUCCEEDED(hc)) {
			hmap = coordinateMapper->MapDepthFrameToColorSpace(512 * 424, depthArray, 512 * 424, colorSpacePoints);
			if (SUCCEEDED(hmap)) {

				int colorPixel = 0;
				for (int y = 0; y < 424; y++) {
					for (int x = 0; x < 512; x++) {
						colorPixel = x + (y * 512);

				
						int xPosition = nearbyintf(colorSpacePoints[colorPixel].X);
						int yPosition = nearbyintf(colorSpacePoints[colorPixel].Y);
						
						if ((yPosition > -1) && (yPosition < 1080) && (xPosition > -1) && (xPosition < 1920)) {
						
							colorArrayMap[4 * (colorPixel)] = colorArray[(4 * (xPosition + (1920 * yPosition)))];
							colorArrayMap[(4 * colorPixel) + 1] = colorArray[(4 * (xPosition + (1920 * yPosition))) + 1];
							colorArrayMap[(4 * colorPixel) + 2] = colorArray[(4 * (xPosition + (1920 * yPosition))) + 2];
							colorArrayMap[(4 * colorPixel) + 3] = colorArray[(4 * (xPosition + (1920 * yPosition))) + 3];
						}
						else {
							colorArrayMap[(4 * colorPixel)] = 'c';
							colorArrayMap[(4 * colorPixel) + 1] = 'c';
							colorArrayMap[(4 * colorPixel) + 2] = 'c';
							colorArrayMap[(4 * colorPixel) + 3] = 'c';
						}
					}
				}
			}
			else cout << "hm fail" << endl;
			string help(colorArrayMap, colorArrayMap + (4 * 512 * 424));
			payload.add_color(help);
		}
		else cout << "-failed to run color copy method for this frame-" << endl;


		for (unsigned int bodyIndex = 0; bodyIndex < _countof(bodies); bodyIndex++) {
			safeRelease(bodies[bodyIndex]);
		}

		for (int i = 0; i < posNOr.size(); i++) {
			payload.add_body(posNOr[i]);
		}

		//WARNING HERE YOU CAN CHOSE INFRA OR DEPTH
		for (int i = 0; i < 217088; i++) {
			payload.add_depth(depthArray[i]);
			//payload.add_depth(infraArray[i]);
		}



		//Google Protobuf
		int siz = payload.ByteSize() + 4;
		char *pkt = new char[siz];
		google::protobuf::io::ArrayOutputStream aos(pkt, siz);
		CodedOutputStream *coded_output = new CodedOutputStream(&aos);
		coded_output->WriteVarint32(payload.ByteSize());
		payload.SerializeToCodedStream(coded_output);

		iResult = send(ConnectSocket, pkt, siz, 0);
		if (iResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}
		safeRelease(depthFrame);

		safeRelease(bodyFrame);
		safeRelease(colorFrame);
		safeRelease(infraFrame);
		safeRelease(colorFrameRef);
		safeRelease(multiFrame);
		safeRelease(bodyFrameRef);
		safeRelease(depthFrameRef);
		payload.clear_body();
		payload.clear_color();
		payload.clear_depth();
		delete(colorSpacePoints);
		delete(coded_output);
		delete(pkt);
		delete(depthArray);
		delete(infraArray);


	}

	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// Receive until the peer closes the connection
	do {

		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
			printf("Bytes received: %d\n", iResult);
		else if (iResult == 0)
			printf("Connection closed\n");
		else
			printf("recv failed with error: %d\n", WSAGetLastError());

	} while (iResult > 0);

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();

	return 0;
}
