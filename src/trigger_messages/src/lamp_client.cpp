#include "ros/ros.h"
#include "std_msgs/String.h"


void lampSwitch(const std_msgs::String::ConstPtr& msg) {
  ROS_INFO("I heard: [%s]", msg->data.c_str());
}


int main(int argc, char **argv) {
  ros::init(argc, argv, "lamp_client");

  ros::NodeHandle n_topic;

  ros::Subscriber lamp = n_topic.subscribe("switcher", 1000, lampSwitch);
  ROS_INFO("Ready to receive signals from server");
  ros::spin();

  return 0;
}
