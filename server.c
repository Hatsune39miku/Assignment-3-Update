#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "network.h"
#include "utils.h"
#include "server.h"
#include <stdio.h>
#include <dirent.h>
#include <pthread.h>


void *server(void *arg){
  serverArg_t *_arg = (serverArg_t *)arg;
  IDlist_t *idList = _arg->idList;
  int portNum = _arg->port;
  char *directory = _arg->directory;
  neighbors_t *neighbors = _arg->neighbors;
  pthread_mutex_t *mutex = _arg->lock;

  int socketfd = createNewSock();
  packet_t packet;
  unsigned long clientIpAddress;

  if (bindSocket(socketfd, portNum) == -1){
    ERROR("Error: wrong port binding. \n");
  }
  printf("[CONNECT] Successfully bind with the port: %d\n", portNum);

  while (1) {
    unsigned char type = packet.descriptor;

    printf("[INFO] Received new packet: ");

    switch(type){
    case 0: printf("CONNECT packet\n"); break;
    case 1: printf("QUERY packet\n"); break;
    case 2: printf("RESPONSE packet\n"); break;
    }

    if (type == CONNECT){
      //TODO: handle server connect

    }
    else if (type == QUERY){
      //TODO: handle server query

    }
    else if (type == RESPONSE){
      //TODO: handle server response

    }
  }
}

/*
 * You may implement the following functions to deal with TODOs. 
 */
int serverHandleConnect(neighbors_t *neighbors, packet_t *packet, pthread_mutex_t *mutex){
  //connect host to neighbors

  //update neighbor list
  return 0;
}


int serverQuery(serverArg_t *args, packet_t *packet) {
  // if found in ID list
  printf("[DUPLICATE ID] Duplicate packet with ID %d detected. Ignoring and discarding.\n", packet->ID);
  
  // if file found
  printf("[RESPON] File %s found, responding to query.\n", packet->message);

  // if file not found
  char neighbor_addr;
  printf("[INFO] File Not found, Forward to next neighbor: %c\n", neighbor_addr);
  
  // else not found, flood to neighbors

  return 0;
}



int serverHandleResponse(packet_t *packet){

  // printf("\n[FILE] File %s found on address %s, with port %d\n\n", 
  //        packet->message,  
  //        getHostAddr(getHostAddr(packet->hostname)),
  //         packet->port);

  return 0;
}
