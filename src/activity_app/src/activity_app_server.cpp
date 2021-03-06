#include "ros/ros.h"
#include "std_msgs/String.h"
#include <sstream>
#include <map>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h> 

//int counter_entry = 0;
bool lampOne = false;
bool lampTwo = false;
bool tv = false;


bool switchLamp(bool toTurnOn, char *host)    {
    printf("CONNECTING %s\n", host);
    
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    
    portno = 23;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        perror("ERROR opening socket");
    server = gethostbyname(host);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        perror("ERROR connecting");
    
    
    
    
    if(toTurnOn)  {
        n = write(sockfd,"1",1);
        //ROS_INFO("[SWITCH] The lamp is going ON!");
    } else  {
        n = write(sockfd,"0",1);
        //ROS_INFO("[SWITCH] The lamp is going OFF!");
    }
    if (n < 0) 
         perror("ERROR writing to socket");
    bzero(buffer,256);
    
    n = read(sockfd,buffer,255);
    if (n < 0) 
         perror("ERROR reading from socket");
    //printf("%s\n",buffer);
    
    
    n = close(sockfd);
    
    if(buffer[0] == '0')  {
      return 0;
    }
    
    return 1;
}

void deviceCallback(const std_msgs::String::ConstPtr& msg) {
  char lampOneHost[] = "143.225.85.168";
  char lampTwoHost[] = "143.225.85.169";
  //char tvHost[] = "143.225.85.170";
  
  if(msg->data.c_str()[0] == '1')	{   //entrata del client nell'area Beacon
      ROS_INFO("Device Entered the Room");
      //counter_entry++;
      //if(counter_entry > 0)    {  //se è entrato il primo client nell'area Beacon, accendo la lampada
          printf("[SERVER] LAMP_ONE: %d\n\n", lampOne = switchLamp(true, lampOneHost));
      //}
  } else if(msg->data.c_str()[0] == '2')  { //avvicinamento a scrivania
    ROS_INFO("Device Approach the Desk");
    printf("[SERVER] LAMP_TWO: %d\n", lampTwo = switchLamp(true, lampTwoHost));
    printf("[SERVER] LAMP_ONE: %d\n", lampOne = switchLamp(false, lampOneHost));
    //printf("[SERVER] TV: %d\n\n", tv = switchLamp(false, tvHost));
    printf("[SERVER] TV: 0\n\n");
  } else if(msg->data.c_str()[0] == '3')  { //avvicinamento al divano
    ROS_INFO("Device Approach the TV");
    printf("[SERVER] LAMP_ONE: %d\n", lampOne = switchLamp(false, lampOneHost));
    printf("[SERVER] LAMP_TWO: %d\n", lampTwo = switchLamp(false, lampTwoHost));
    //printf("[SERVER] TV: %d\n\n", tv = switchLamp(true, tvHost));
    printf("[SERVER] TV: 1\n\n");
  } else if(msg->data.c_str()[0] == '4')  { //allontanamento dalla scrivania
    ROS_INFO("Device Removal the Desk or the Couch");
    printf("[SERVER] LAMP_ONE: %d\n", lampOne = switchLamp(true, lampOneHost));
    printf("[SERVER] LAMP_TWO: %d\n", lampTwo = switchLamp(false, lampTwoHost));
    //printf("[SERVER] TV: %d\n\n", tv = switchLamp(false, tvHost));
    printf("[SERVER] TV: 0\n\n");
  } else    {   //uscita del client dall'area Beacon
      ROS_INFO("Device Exited the Room");
      //counter_entry--; 
      //if (counter_entry == 0)   {   //se non ci sono più client nell'area Beacon, spengo la lampada 
        printf("[SERVER] LAMP_ONE: %d\n\n", lampOne = switchLamp(false, lampOneHost));
      //}
  }
  //printf("[DEBUG] counter_entry = %d\n", counter_entry);
}


int main(int argc, char **argv) {  
  char topicName[] = "recogniser";
  
  ros::init(argc, argv, "listener");

  ros::NodeHandle node;
  
  ros::Subscriber sub = node.subscribe(topicName, 1000, deviceCallback);

  ROS_INFO("[SERVER] Ready to receive clients\n");
  ros::spin();

  return 0;
}
