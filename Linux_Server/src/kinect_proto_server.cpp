#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <netinet/in.h>
#include <resolv.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include "kinect_message.pb.h"
#include <iostream>
#include <google/protobuf/io/coded_stream.h>
//#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/image_encodings.h>
#include <visualization_msgs/MarkerArray.h>
#include <sensor_msgs/fill_image.h>
#include <sensor_msgs/CameraInfo.h>
#include <cv_bridge/cv_bridge.h>
#include <opencv-3.2.0-dev/opencv2/opencv.hpp>
#include <sensor_msgs/distortion_models.h>
#include <inttypes.h>
using namespace std;
using namespace google::protobuf::io;

void* SocketHandler(void*);
sensor_msgs::CameraInfo fill_cam_info();
ros::Time syncerino;

//global cause i want to use them in the nested functions without giving them as parameters everytime
 
ros::Publisher depth0Pub;
ros::Publisher depth1Pub;
ros::Publisher depth2Pub;
ros::Publisher depth3Pub;

ros::Publisher rgb0Pub;
ros::Publisher rgb1Pub;
ros::Publisher rgb2Pub;
ros::Publisher rgb3Pub;
ros::Publisher camPub;

vector<ros::Publisher> bodyPub0;
vector<ros::Publisher> bodyPub1;
vector<ros::Publisher> bodyPub2;
vector<ros::Publisher> bodyPub3;




int main(int argc, char** argv){
        //RosStuff
        ros::init(argc, argv, "kinect_proto_server");
        ros::NodeHandle nh;
        depth0Pub= nh.advertise<sensor_msgs::Image>("image_rect0",1000);
        depth1Pub = nh.advertise<sensor_msgs::Image>("image_rect1", 1000);
        depth2Pub = nh.advertise<sensor_msgs::Image>("image_rect2", 1000);
        depth3Pub = nh.advertise<sensor_msgs::Image>("image_rect3", 1000);
        
        rgb0Pub = nh.advertise<sensor_msgs::Image>("image_rect_color0", 1000);
        rgb1Pub = nh.advertise<sensor_msgs::Image>("image_rect_color1", 1000);
        rgb2Pub = nh.advertise<sensor_msgs::Image>("image_rect_color2", 1000);
        rgb3Pub = nh.advertise<sensor_msgs::Image>("image_rect_color3", 1000);

        camPub = nh.advertise<sensor_msgs::CameraInfo>("camera_info", 1000);
        
        for(int i=0; i<6;i++){
          bodyPub0.push_back(nh.advertise<visualization_msgs::MarkerArray>("body_0"+to_string(i), 1000));
          bodyPub1.push_back(nh.advertise<visualization_msgs::MarkerArray>("body_1"+to_string(i), 1000));
          bodyPub2.push_back(nh.advertise<visualization_msgs::MarkerArray>("body_2"+to_string(i), 1000));
          bodyPub3.push_back(nh.advertise<visualization_msgs::MarkerArray>("body_3"+to_string(i), 1000));
        }
        
        log_packet loggi;
        int host_port= 1101;

        struct sockaddr_in my_addr;

        int hsock;
        int * p_int ;
        int err;

        socklen_t addr_size = 0;
        int* csock;
        sockaddr_in sadr;
        pthread_t thread_id=0;

        hsock = socket(AF_INET, SOCK_STREAM, 0);
        if(hsock == -1){
                printf("Error initializing socket %d\n", errno);
                goto FINISH;
        }

        p_int = (int*)malloc(sizeof(int));
        *p_int = 1;

        if( (setsockopt(hsock, SOL_SOCKET, SO_REUSEADDR, (char*)p_int, sizeof(int)) == -1 )||
                (setsockopt(hsock, SOL_SOCKET, SO_KEEPALIVE, (char*)p_int, sizeof(int)) == -1 ) ){
                printf("Error setting options %d\n", errno);
                free(p_int);
                goto FINISH;
        }
        free(p_int);

        my_addr.sin_family = AF_INET ;
        my_addr.sin_port = htons(host_port);

        memset(&(my_addr.sin_zero), 0, 8);
        my_addr.sin_addr.s_addr = INADDR_ANY ;

        if( bind( hsock, (sockaddr*)&my_addr, sizeof(my_addr)) == -1 ){
                fprintf(stderr,"Error binding to socket, make sure nothing else is listening on this port %d\n",errno);
                goto FINISH;
        }
        if(listen( hsock, 10) == -1 ){
                fprintf(stderr, "Error listening %d\n",errno);
                goto FINISH;
        }

        //Now lets do the server stuff

        addr_size = sizeof(sockaddr_in);
        //ros::Rate loop_rate(10);
        while(ros::ok()){
                printf("waiting for a connection\n");
                csock = (int*)malloc(sizeof(int));
                if((*csock = accept( hsock, (sockaddr*)&sadr, &addr_size))!= -1){
                        printf("---------------------\nReceived connection from %s\n",inet_ntoa(sadr.sin_addr));
                        pthread_create(&thread_id,0,&SocketHandler, (void*)csock );
                        pthread_detach(thread_id);
                }
                else{
                        fprintf(stderr, "Error accepting %d\n", errno);
                }
        }

FINISH:
;//oopss
}
sensor_msgs::CameraInfo fill_cam_info(){
    sensor_msgs::CameraInfo kinect_camera_info;
    kinect_camera_info.width = 512;
    kinect_camera_info.header.frame_id="map";
    syncerino = ros::Time::now();
    kinect_camera_info.header.stamp = syncerino;
    kinect_camera_info.height = 424;
    kinect_camera_info.distortion_model = sensor_msgs::distortion_models::PLUMB_BOB;
/*
    kinect_camera_info.D[0]=0.0936385914683342;
    kinect_camera_info.D[0]=-2.708786129951477;
    kinect_camera_info.D[0]=0.0;
    kinect_camera_info.D[0]=0.0;
    kinect_camera_info.D[0]=0.09204775940044022;
  */  

    kinect_camera_info.R[0]=1.0;
    kinect_camera_info.R[1]= 0.0;
    kinect_camera_info.R[2]= 0.0;
    kinect_camera_info.R[3]= 0.0;
    kinect_camera_info.R[4]= 1.0;
    kinect_camera_info.R[5]= 0.0;
    kinect_camera_info.R[6]= 0.0;
    kinect_camera_info.R[7]= 0.0;
    kinect_camera_info.R[8]= 1.0;

    kinect_camera_info.P[0]=365.580810546875;
    kinect_camera_info.P[1]= 0.0;
    kinect_camera_info.P[2]= 258.11810302734375;
    kinect_camera_info.P[3]= 0.0;
    kinect_camera_info.P[4]= 0.0;
    kinect_camera_info.P[5]= 365.580810546875;
    kinect_camera_info.P[6]= 206.75650024414062;
    kinect_camera_info.P[7]= 0.0;
    kinect_camera_info.P[8]= 0.0;
    kinect_camera_info.P[9]= 0.0;
    kinect_camera_info.P[10]= 1.0;
    kinect_camera_info.P[11]= 0.0;

    kinect_camera_info.K[0]=365.580810546875;
    kinect_camera_info.K[1]=0.0;
    kinect_camera_info.K[2]=258.11810302734375;
    kinect_camera_info.K[3]=0.0;
    kinect_camera_info.K[4]=365.580810546875;
    kinect_camera_info.K[5]=206.75650024414062;
    kinect_camera_info.K[6]=0.0;
    kinect_camera_info.K[7]=0.0;
    kinect_camera_info.K[8]=1.0;
    
    
    
    
    
    
    return kinect_camera_info;
}


google::protobuf::uint32 readHdr(char *buf)
{
  google::protobuf::uint32 size;
  google::protobuf::io::ArrayInputStream ais(buf,4);
  CodedInputStream coded_input(&ais);
  coded_input.ReadVarint32(&size);//Decode the HDR and get the size
  //cout<<"size of payload is "<<size<<endl;
  return size;
}
sensor_msgs::Image makeRgbImage(char* colorBytes,int kinectnum){
        string kin = "kin";
        kin.append(to_string(kinectnum));
        sensor_msgs::Image colorImage;
        colorImage.height=424;
        
        
        colorImage.width=512;
        //colorImage.encoding=sensor_msgs::image_encodings::BGRA8;
        /*
        sensor_msgs::fillImage(colorImage,
                        sensor_msgs::image_encodings::BGRA8,
                        424,
                        512,
                        2048,
                        colorBytes);
        */
        cv::Mat colorMat = cv::Mat(424,512,CV_8UC4,colorBytes).clone();
        
        //cv::imshow("bla",colorMat);
        //cv::waitKey(1);
        cv::Mat matter(424,512,CV_8UC4);
        cv::flip(colorMat, matter, 1);
     
       
        cv_bridge::CvImage cvi_mat;
        cvi_mat.encoding = sensor_msgs::image_encodings::BGRA8;
        cvi_mat.image = matter;
        cvi_mat.toImageMsg(colorImage); 
        colorImage.header.frame_id=kin;

        return colorImage;
}

sensor_msgs::Image makeDepthImage(vector<uint16_t>depthVec,int kinectnum,ros::Time stamp){
        string kin = "kin";
        kin.append(to_string(kinectnum));
        sensor_msgs::Image depthImage;

        
        

        cv::Mat matter(424,512,CV_16UC1);
        cv::Mat monchich (424,512,CV_16UC1,depthVec.data());

        cv::flip(monchich,matter,1);

        cv_bridge::CvImage cvi_mat;
        cvi_mat.encoding = sensor_msgs::image_encodings::TYPE_16UC1;
        cvi_mat.image = matter;
        cvi_mat.toImageMsg(depthImage);  

        depthImage.header.frame_id=kin;
        //depthImage.header.stamp.sec=stamp.sec;
        //depthImage.header.stamp.nsec=stamp.nsec;
        depthImage.header.stamp = syncerino;
        depthImage.height=424;
        depthImage.width=512;
        
        return depthImage;

}

void pubBodies(int kinectNum,vector<double>bodyVec,int bodycount,vector<ros::Publisher> pubvec, ros::Time stamp){
    //number of markerarrays for this kinect
    //xyzwxyz*25*(6)
    vector<visualization_msgs::MarkerArray> marker_array_vector;
    
    //there are #bodycount entries in the marker_array_vector
    int k=0;
    for(int i=0;i<bodycount;i++){
        
        visualization_msgs::MarkerArray marker_array;
        marker_array.markers.resize(25);
        

      int count=0;
      for(int j=k;j<(k+175);j+=7){
          visualization_msgs::Marker marker;
          marker.header.frame_id= "kin"+to_string(kinectNum);
          marker.pose.position.x = bodyVec[j];
          //cout<<"bodyvec[i]"<<-bodyVec[i]<<endl;
          marker.pose.position.y = bodyVec[j+1];
          marker.pose.position.z = bodyVec[j+2];

          marker.pose.orientation.w = bodyVec[j+3];
          marker.pose.orientation.x = bodyVec[j+4];
          marker.pose.orientation.y = bodyVec[j+5];
          marker.pose.orientation.z = bodyVec[j+6];
          ros::Duration seconds(0.1);
          marker.lifetime = seconds;

          marker.type=visualization_msgs::Marker::SPHERE;
          if(count==0)marker.action =0;
          
          //marker.header.stamp = stamp;
          marker.id=j;
          marker.scale.x = 0.05;
          marker.scale.y = 0.05;
          marker.scale.z = 0.05;
          marker.color.a = 1.0;
          marker.color.r = 255;
          marker.color.g = 255;
          marker.color.b = 255;
          marker_array.markers[count]=marker;
 
          count++;

         }
         k+=175;
         marker_array_vector.push_back(marker_array);
    }
    for(int i = 0; i<bodycount; i++){
       pubvec[i].publish(marker_array_vector[i]);
    }
    return;
}

void readBody(int csock,google::protobuf::uint32 siz)
{
  int bytecount;
  int kinectNum;
  ros::Time stamp;
  
  
  log_packet payload;
  char buffer [siz+4];//size of the payload and hdr
  //Read the entire buffer including the hdr
  if((bytecount = recv(csock, (void *)buffer, 4+siz, MSG_WAITALL))== -1){
                fprintf(stderr, "Error receiving data %d\n", errno);
        }
  //cout<<"Second read byte count is "<<bytecount<<endl;
  //Assign ArrayInputStream with enough memory 
  google::protobuf::io::ArrayInputStream ais(buffer,siz+4);
  CodedInputStream coded_input(&ais);
  //Read an unsigned integer with Varint encoding, truncating to 32 bits.
  coded_input.ReadVarint32(&siz);
  //After the message's length is read, PushLimit() is used to prevent the CodedInputStream 
  //from reading beyond that length.Limits are used when parsing length-delimited 
  //embedded messages
  google::protobuf::io::CodedInputStream::Limit msgLimit = coded_input.PushLimit(siz);
  //De-Serialize
  payload.ParseFromCodedStream(&coded_input);
  //Once the embedded message has been parsed, PopLimit() is called to undo the limit
  coded_input.PopLimit(msgLimit);

 // google::protobuf::RepeatedPtrField<string> ptrfield=payload.color();
  google::protobuf::RepeatedField<google::protobuf::uint32> depthField= payload.depth();
  google::protobuf::RepeatedField<double> bodyField= payload.body();

  vector<uint16_t> copyVecDepth;
  vector<double> copyVecBody;

  //find the delimiter for end of message
  int index=0;
  
  while(true){
        if(bodyField.Get(index)==INFINITY){
                break;
        }else index++;
  }
  int bodycount=0;
  if(index>5)bodycount=(index-3)/(25*7);
 

  


  kinectNum=bodyField.Get(0);
  stamp=ros::Time((int)bodyField.Get(1),(int)bodyField.Get(2));
  
  
  //cout<<kinectNumber<<" NUmerooo"<<endl;

  for(int i =3;i<index;i++){
       // cout<<bodyField.Get(i)<<"dings"<<endl;
        copyVecBody.push_back(bodyField.Get(i));
  }



/*
  //safe color into string
  string simonsagt;
  simonsagt.reserve(217088*4);
  char* colorVector= new  char[217088*4];
  //cv::imshow("nafti",*ptrfield.Get(0).c_str());
  //string inge= ptrfield.Get(0).c_str();
  for(int i=0;i<4;i++){
          simonsagt.append(ptrfield.Get(i));
  }
  colorVector=&*simonsagt.begin();
 */
  //cv::waitKey(1);

  //safe the depth vector into array
  for(int i =0;i<217088;i++){
        copyVecDepth.push_back(depthField.Get(i));

  }
  //MakeDepthImage
  camPub.publish(fill_cam_info());
  switch(kinectNum){
        case 0:
        pubBodies(0,copyVecBody,bodycount,bodyPub0,stamp);
        depth0Pub.publish(makeDepthImage(copyVecDepth,kinectNum,stamp));
        //rgb0Pub.publish(makeRgbImage(colorVector,kinectNum));
        break;
        case 1:
        pubBodies(1,copyVecBody,bodycount,bodyPub1,stamp);
        depth1Pub.publish(makeDepthImage(copyVecDepth,kinectNum,stamp));
      //  rgb1Pub.publish(makeRgbImage(colorVector,kinectNum));
        break;
        case 2:
        pubBodies(2,copyVecBody,bodycount,bodyPub2,stamp);
        depth2Pub.publish(makeDepthImage(copyVecDepth,kinectNum,stamp));
        //rgb2Pub.publish(makeRgbImage(colorVector,kinectNum));
        break;
        case 3:
        pubBodies(3,copyVecBody,bodycount,bodyPub3,stamp);
        depth3Pub.publish(makeDepthImage(copyVecDepth,kinectNum,stamp));
       //rgb3Pub.publish(makeRgbImage(colorVector,kinectNum));

  }
  





  //string bla;
  //copy(ptrfield.Get(0),ptrfield.Get((217088*4)-1),bla);
 //const  char *bla=ptrfield.data();

}

void* SocketHandler(void* lp){
    int *csock = (int*)lp;

        char buffer[4];
        int bytecount=0;
        string output,pl;
        log_packet logp;

        memset(buffer, '\0', 4);
       // ros::Rate loop_rate(30);
        while (1) {
        //Peek into the socket and get the packet size
        if((bytecount = recv(*csock,
                         buffer,
                                 4, MSG_PEEK))== -1){
                fprintf(stderr, "Error receiving data %d\n", errno);
        }else if (bytecount == 0)
                break;
        //cout<<"First read byte count is "<<bytecount<<endl;
        readBody(*csock,readHdr(buffer));
        ros::spinOnce();
        //loop_rate.sleep();
        }

FINISH:
        free(csock);
    return 0;
}