#include "utils.h"
#include "server.h"
#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {
    if (argc < 4) {
        ERROR("ERROR: Insufficient arguments provided. Usage: ./p2p_server [PORT_NUM] [TTL] [DIR] [NEIGHBOR_HOST, ...]\n");
    }

    int myPortNumber = atoi(argv[1]);
    int ttl = atoi(argv[2]);
    char directory[128];
    strcpy(directory, argv[3]);

    neighbors_t neighbors;
    IDlist_t idList;

    initializeNeighbors(&neighbors);
    initializeList(&idList); 
    for (int i = 4; i < argc; i += 2) {
        unsigned long hostIpAddress = getHostAddr(argv[i]);
        int portNumber = atoi(argv[i + 1]);
        addNeighbor(&neighbors, hostIpAddress, portNumber);
    }

    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);

    serverArg_t serverArg = { .idList = &idList, .port = myPortNumber, .directory = directory, .lock = &lock, .neighbors = &neighbors };

    pthread_t serverThreadId;
    if (pthread_create(&serverThreadId, NULL, server, (void *)&serverArg) != 0) {
        ERROR("Failed to create the server thread\n");
    }


    while (1) {
        printf("[FILE] Enter the File Name for Search: \n");
        char input[MAX_STRLEN];
        fgets(input, MAX_STRLEN, stdin);
        int len = strlen(input);
        if (input[len - 1] == '\n') input[len - 1] = '\0';

        if (findInDirectory(directory, input)) {
            printf("[FILE] Found file '%s' in local directory \n", input);
            continue;
        }

        packet_t packet;
        generatePacket(&packet, input, QUERY, ttl);
        floodRequest(&neighbors, myPortNumber, &packet, sizeof(packet));
    }

    pthread_join(serverThreadId, NULL);
    pthread_mutex_destroy(&lock);

    return 0;
}


