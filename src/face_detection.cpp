# include <ros/ros.h>
# include <cv_bridge/cv_bridge.h>
# include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/opencv.hpp"
#include <stdio.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include <image_transport/image_transport.h>
#include <sensor_msgs/image_encodings.h>					
#include <sstream>
#include "std_msgs/String.h"

using namespace std;
using namespace cv;


class camera_test
{ 
  
  

	public:
	
	 camera_test()														// constructor camera_test 

	{		
		ros::NodeHandle nh;												// creating a node for ROS communication with oother nodes
		image_transport::ImageTransport it(nh);
 		image_transport::Publisher pub = it.advertise("camera/image", 1);
		ros::Publisher name_pub = nh.advertise<std_msgs::String>("chatter_name", 1000);
		std_msgs::String msg_text;
		stringstream ss;
		ss<<"hello";
		msg_text.data=ss.str();
		ROS_INFO_STREAM("string is"<<msg_text.data.c_str());
		name_pub.publish(msg_text);
		
		string text = "ROSHAN";
		int fontFace = FONT_HERSHEY_SCRIPT_SIMPLEX;
		double fontScale = 3;
		int thickness = 3;
		int baseline=0;
		Size textSize = getTextSize(text, fontFace,fontScale, thickness, &baseline);
		baseline += thickness;

		VideoCapture cap(0);										        	// object creation for VideoCapture with camera port(0-default,1-external)
		String face_cascade_name = "/home/roshan/catkin_ws/src/face_detection/src/cascade.xml";				// specify the path for the face cascade.xml file	
		String eye_cascade_name="/home/roshan/catkin_ws/src/face_detection/src/cascade_eye.xml";
		CascadeClassifier face_cascade;											// object creation for CascadeClassifier
		face_cascade.load(face_cascade_name);
		CascadeClassifier eye_cascade;
		eye_cascade.load(eye_cascade_name);

		if(!face_cascade.load(face_cascade_name))
		{	
			ROS_INFO("cannot open cascade.xml");
 		} 
		if (!cap.isOpened())												// check if camera is opened
		{
			ROS_INFO("Cannot open camera");
		}

		double cam_width= cap.get(CV_CAP_PROP_FRAME_WIDTH);
		double cam_length= cap.get(CV_CAP_PROP_FRAME_HEIGHT);						// get the camera height, width and fps using the VideoCapture object and get() function
		double fps = cap.get(CV_CAP_PROP_FPS);
		
		cout<<"frame size" << cam_width << "x" << cam_length;
		cout << "Frame per seconds : " << fps << endl;									// print the functions

		namedWindow("Video Preview", CV_WINDOW_AUTOSIZE);								// Name the window as "video preview"

		ros::Rate loop_rate(5);
		while (nh.ok())
		{
			string name="ROSHAN";
			Mat frame;
			std::vector<Rect> faces;
			Mat frame_gray;										// Creation of frame( normal frame capture) and frame-gray( grayscale image) as a Mat 
			sensor_msgs::ImagePtr msg;
			std::vector<Rect> eyes;
			//vector<Mat> images;
  			//vector<int> labels;
			bool rval= cap.read(frame);								// check if rval is -1( true) or 0(false)
			resize(frame, frame, Size(cam_width, cam_length), 0, 0, INTER_CUBIC);			// resize the image as you want (remove the variables and plug in cals, 640x480.....)
			if( ! rval)
			{
			cout << " cant read the frame";
			break;
			}
			else
			{
			ROS_INFO(" face detection loop inside");			
			cvtColor( frame, frame_gray, CV_BGR2GRAY);  												// convert to a grayscale
			equalizeHist(frame_gray, frame_gray);
			face_cascade.detectMultiScale( frame_gray, faces, 1.1, 4, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );					// face detection 
			for( size_t i = 0; i < faces.size(); i++ )												// draw rectangles on the detected faces
			{
			 	Point center( faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5 );
				//ellipse( frame, center, Size( faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar( 255, 0, 255 ), 3, 8, 0 );
				Mat faceROI = frame_gray( faces[i] );
			    	Rect face_i = faces[i];
			 	//   int pos_x = std::max(face_i.tl().x - 10, 0);
         		 	//   int pos_y = std::max(face_i.tl().y - 10, 0); 
			   	Point textorg((faces[i].x)/2 ,(faces[i].y)/2 );
			   	putText(frame, name, textorg, FONT_HERSHEY_SIMPLEX,0.5, (255, 255, 255),2);									
			    	rectangle(frame, face_i, CV_RGB(0, 255,0), 3);
			    
			      			
   			eye_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(30, 30) );
   			for( size_t j = 0; j < eyes.size(); j++ )
   				 {
    				   Point center( faces[i].x + eyes[j].x + eyes[j].width*0.5, faces[i].y + eyes[j].y + eyes[j].height*0.5 );
     				   int radius = cvRound( (eyes[j].width + eyes[j].height)*0.25 );
   				   circle( frame, center, radius, Scalar( 255, 0, 0 ), 2, 8, 0 );
   				  }    	
			
	 		 }
			sensor_msgs::ImagePtr msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", frame_gray).toImageMsg();			// Image transport from openCV to ROS with specific encoding
			pub.publish(msg);													//Publish msg to "/camera/image"

	
																		  
			imshow("Video Preview", frame);														// Show the image on screen
				
			}

			if ( waitKey(10)== 27 || waitKey(10) == 3 )						//if keyboard interruption is for 20ms, 27 ASCII for ESC and 3 for ctrl+c,close frame	
			{
				cout<<"Interruption by keyboard";
				break;
			}
		}
	}

~camera_test()																			// destructor invoked
	{
		cvDestroyWindow("Windows Closed"); 
	}
};





int main( int argc, char **argv)
{
	ros::init(argc, argv, "Camera_test");			// initialize ros for command line arguements
	camera_test cam;						// creating the object for the class camera_test
	ROS_INFO("Camera Tested");
	ros::spin();

}
