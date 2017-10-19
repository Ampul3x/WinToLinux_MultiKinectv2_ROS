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
	std::cout << "Please enter number of kinect (0 for first)" << std::endl;
	std::cin >> kinectCount;
	cv::Mat image;
	//Kinect
	IKinectSensor *sensor = nullptr;
	IBodyFrameReader *bodyFrameReader = nullptr;
	IDepthFrameReader *frameReader = nullptr;
	IColorFrameReader *colorFrameReader = nullptr;
	byte* colorArray = new byte[1920*1080*4];
	UINT size = 217088;
	UINT16 *buffer = new UINT16[217088];
	IBodyFrameSource *bodyFrameSource = nullptr;
	IDepthFrameSource *depthFrameSource = nullptr;
	IColorFrameSource *colorFrameSource = nullptr;

	//Get the default Kinect sensor
	HRESULT hs;
	HRESULT hc;
	HRESULT hr = GetDefaultKinectSensor(&sensor);
	
	if (SUCCEEDED(hr)) {
		sensor->Open();
	}
	if (SUCCEEDED(hr)) {
		hs = hr;
		hc = hr;
	}
	//Get DepthFramereader and BodyFrameReader and Color
	hr = sensor->get_BodyFrameSource(&bodyFrameSource);
	hs = sensor->get_DepthFrameSource(&depthFrameSource);
	hc = sensor->get_ColorFrameSource(&colorFrameSource);

	if (SUCCEEDED(hr)) {
		hr = bodyFrameSource->OpenReader(&bodyFrameReader);
	}

	if (SUCCEEDED(hs)) {
		hs = depthFrameSource->OpenReader(&frameReader);
		if (SUCCEEDED(hs)) {
			std::cout << "depthFrameSource->OpenReader OK" << std::endl;

		}
	}
	if (SUCCEEDED(hc)) {
		hc = colorFrameSource->OpenReader(&colorFrameReader);

	}
	if (sensor == nullptr || FAILED(hr) || FAILED(hs)||FAILED(hc)) {

		return E_FAIL;
	}

	safeRelease(bodyFrameSource);
	safeRelease(depthFrameSource);
	safeRelease(colorFrameSource);
	//int host_port = 1101;
	//char* host_name = "134.106.37.255";
	char* host_name = "192.168.0.104";
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;
	char *sendbuf = "this is a test";
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;
	cout << host_name <<"  is the ip adress"<< endl;

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
		WSACleanup();
		return 1;
	}
	
	
	
	
	
	
	//Loop
	while ((bodyFrameReader != nullptr) && (frameReader != nullptr)&&(colorFrameReader!=nullptr)) {
		
		byte resizedColor[4*217088]; 
		log_packet payload;
		SYSTEMTIME time ;
		FILETIME ft_now;
		GetSystemTime(&time);
		SystemTimeToFileTime(&time, &ft_now);

		LONGLONG ll_now = (LONGLONG)ft_now.dwLowDateTime + ((LONGLONG)(ft_now.dwHighDateTime) << 32LL);

		int seconds = ll_now / 10000000;
		int nanoseconds = (ll_now  % 10000000)*100;
	
		IBodyFrame *bodyFrame = nullptr;
		IDepthFrame *depthFrame = nullptr;
		IColorFrame *colorFrame = nullptr;

		IBody *bodies[BODY_COUNT] = { 0 };
		BOOLEAN isTracked = false;
		UINT16* depthArray = new UINT16[217088];
		hr = bodyFrameReader->AcquireLatestFrame(&bodyFrame);
		

	
		//here,the code waits 4 e_pending
		while ((FAILED(hr))) {

			Sleep(1);
			hr = bodyFrameReader->AcquireLatestFrame(&bodyFrame);
		}
	
		hr = bodyFrame->GetAndRefreshBodyData(_countof(bodies), bodies);
		hs = frameReader->AcquireLatestFrame(&depthFrame);

	

		if(SUCCEEDED(hs))hs = depthFrame->CopyFrameDataToArray(size, depthArray);
		
		//COLOR IS DISABLED
		/*
		hc = colorFrameReader->AcquireLatestFrame(&colorFrame);
		
		if(SUCCEEDED(hc)){

			hc = colorFrame->CopyConvertedFrameDataToArray(1920*1080*4,colorArray,ColorImageFormat_Bgra);
			image=cv::Mat(1080, 1920,CV_8UC4,colorArray).clone();
			cv::Rect myRoi(308, 0, 1920 - 616, 1080);
			//cv::Mat croppedImage = image(myRoi);
			cv::resize(image, image, cv::Size(512,424),1, cv::INTER_LANCZOS4);
			int sizl = image.total() * image.elemSize();
	
			
			std::memcpy(resizedColor, image.data, sizl* sizeof(byte));
			
			imshow("color", image);
			cv::waitKey(1);
		
		}
		else cout << "color does not work" << endl;
		*/
		
		

		//in the bodyVector posNOr, the timestamps and the kinectcounts are saved.
		vector<double> posNOr;
		posNOr.push_back(kinectCount);
		posNOr.push_back(seconds);
		posNOr.push_back(nanoseconds);
	


		
		for (int bodyIndex = 0; bodyIndex < BODY_COUNT; bodyIndex++) {
			IBody *body = bodies[bodyIndex];
			HRESULT hr = body->get_IsTracked(&isTracked);
			if (FAILED(hr)) {
				continue;
			}
			else if (isTracked == false) {
				//the body was not found
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

			}
		
			
		}
		//the message is over a deliminter is inserted
		posNOr.push_back(INFINITY);

		for (unsigned int bodyIndex = 0; bodyIndex < _countof(bodies); bodyIndex++) {
			safeRelease(bodies[bodyIndex]);
		}

		//Need to release the bodies
		for (int i = 0; i < posNOr.size(); i++) {
			payload.add_body(posNOr[i]);
			

		}
		for (int i = 0; i < 217088; i++) {
			payload.add_depth(depthArray[i]);
		}
		//COLOR is Disabled
		/*
		for (int i = 0; i < 217088*4; i=i+217088) {
			string help(&resizedColor[i], &resizedColor[i+217088]);
			payload.add_color(help);
		}
		*/
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
		payload.clear_body();
		payload.clear_color();
		payload.clear_depth();
		delete(coded_output);
		delete(pkt);
		delete(depthArray);
		//delete[](resizedColor);
		
	}

	//printf("Bytes Sent: %ld\n", iResult);

	// shutdown the connection since no more data will be sent
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
