#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ball_chaser::DriveToTarget srv;
    
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z; 
  
    // Call the command robot and pass the requested joint angles
    if (!client.call(srv))
        ROS_ERROR("Failed to call service command_robot");
  
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;

    // Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
  
  
    // set the limit where  left, forward and right sections of the image end
    int limit_left = img.step*0.3;
    int limit_mid = img.step*0.7;
    int limit_right = img.step;
       
    // Number of pixels in each section
    int no_pixel_left = 0;
    int no_pixel_right = 0;
    int no_pixel_mid = 0;
    int no_total_pixels = 0;
  
    int max_val = 0;
    // treverse the image counting white pixels  
    for (int i = 0; i <img.height; i++)
       for (int j=0; j < img.step; j++){ 
          int index = i*img.height+j;
          if (img.data[index]>max_val)
            max_val = img.data[index];
          if ((img.data[index]==white_pixel) && (j < limit_left))
            ++no_pixel_left;
          else if ((img.data[index]==white_pixel) && (j < limit_mid))
            ++no_pixel_mid;
          else if ((img.data[index]==white_pixel) && (j < limit_right))
            ++no_pixel_right;
          ++no_total_pixels;
       }
    
        
    printf ("%d %d %d %d %d\n", no_pixel_right, no_pixel_mid, no_pixel_left, no_total_pixels,  max_val);
  
    if (no_pixel_left > no_pixel_mid)
       drive_robot (0.1, -0.1);
    else if (no_pixel_right > no_pixel_mid)
       drive_robot (0.1, 0.1);
    else if (no_pixel_mid != 0)
       drive_robot (0.1, 0.0);
    else 
       drive_robot(0.0, 0.0);
        
}

int main(int argc, char** argv)
{
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
