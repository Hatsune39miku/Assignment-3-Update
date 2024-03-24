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

    if (type == CONNECT) {
    pthread_mutex_lock(mutex);
    unsigned long neighborIP = getHostAddr(packet.hostname); // 获取hostname对应的IP地址
    addNeighbor(neighbors, neighborIP, packet.port);
    pthread_mutex_unlock(mutex);
    printf("[CONNECT] Connection established with %s:%d\n", packet.hostname, packet.port);
}


  

    else if (type == QUERY) {
    pthread_mutex_lock(mutex);
    printf("QUERY packet\n");
        pthread_mutex_lock(mutex);
        if (!findInList(idList, packet.ID)) {
            addToDataList(idList, packet.ID);
            pthread_mutex_unlock(mutex);
            if (findInDirectory(directory, packet.message)) {
                printf("[RESPONSE] File %s found, responding to query.\n", packet.message);
            } else if (packet.TTL > 1) {
                packet.TTL -= 1;
                floodRequest(neighbors, _arg->port, &packet, sizeof(packet));
                printf("[FORWARD] File %s not found, forwarding query.\n", packet.message);
            }
        } else {
            pthread_mutex_unlock(mutex);
        }
        break;
}


    else if (type == RESPONSE) {
        printf("RESPONSE packet\n");
        printf("[RESPONSE] File '%s' found at %s:%d\n", packet.message, packet.hostname, packet.port);
        break;
}

  }
}


/*
 * You may implement the following functions to deal with TODOs. 
 */
int serverHandleConnect(neighbors_t *neighbors, packet_t *packet, pthread_mutex_t *mutex) {
    pthread_mutex_lock(mutex);
    unsigned long host_in_addr = inet_addr(packet->hostname);
    addNeighbor(neighbors, host_in_addr, packet->port);
    pthread_mutex_unlock(mutex);
    return 0;
}




int serverQuery(serverArg_t *args, packet_t *packet) {
    pthread_mutex_lock(args->lock);
    if (findInList(args->idList, packet->ID)) {
        pthread_mutex_unlock(args->lock);
        printf("[DUPLICATE ID] Duplicate packet with ID %d detected. Ignoring and discarding.\n", packet->ID);
        return 0;
    }
    addToDataList(args->idList, packet->ID);
    pthread_mutex_unlock(args->lock);

    if (findInDirectory(args->directory, packet->message)) {
        printf("[RESPONSE] File %s found, responding to query.\n", packet->message);
        
        packet_t responsePacket;
        responsePacket.ID = packet->ID;  
        responsePacket.descriptor = RESPONSE;  
        strncpy(responsePacket.message, packet->message, MAX_STRLEN);  
        strncpy(responsePacket.hostname, "", MAX_STRLEN);  
        responsePacket.TTL = packet->TTL - 1; 
        responsePacket.port = args->port;  


        unsigned long host_inaddr = getHostAddr(packet->hostname);

        sendToSocket(host_inaddr, packet->port, &responsePacket, sizeof(responsePacket));
    } else if (packet->TTL > 1) {
        packet->TTL -= 1;
        printf("[INFO] File %s not found, forwarding query to neighbors.\n", packet->message);

    }

    return 0;
}




int serverHandleResponse(packet_t *packet) {
    printf("[FILE] File %s found on address %s, with port %d\n", packet->message, packet->hostname, packet->port);
    return 0;
}
