#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
      ROS_INFO_STREAM("Set linear and angular velocity");
     ball_chaser::DriveToTarget drive_service;
     drive_service.request.linear_x = lin_x;
     drive_service.request.angular_z = ang_z;
  
      if (!client.call(drive_service)) {
        ROS_ERROR("Failed to call service safe_move");
      }
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    int width = img.width;
    int height = img.height;
    int col_sum = 0;
    int num_white = 0;
    int third_img = width/3;
    

    // TODO: Loop through each pixel in the image and check if there's a bright white one

        for (int i = 0; i < height*width; i=i+3)
        {   
            if (img.data[i] == white_pixel && img.data[i+1] == white_pixel && img.data[i+2] == white_pixel)
            {   
           
                int col = (i % (width*3)) / 3;
                col_sum += col;
                num_white += 1;
            }
		}
  
  if (num_white == 0)  {
    drive_robot(0.0, 0.0);
  }
  else {
                 int column_found = col_sum/num_white;
  
                  // Left turn     
                  if (column_found < third_img) {
                drive_robot(0.1, 0.2);  
                  }
                // Right turn
                else if (column_found > (2*third_img)) {
                drive_robot(0.1, -0.2);
                }
                // Straight ahead
                else { 
                drive_robot(0.2, 0.0); 
                }
             
  }
   
}


int main(int argc, char** argv) {
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}