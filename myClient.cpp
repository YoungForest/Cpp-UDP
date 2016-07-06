#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#include <iomanip>
#include <string>

#define BUFLEN 2048
#define IPLEN 20

using namespace std;

void * receiveMessage(void * argument);

struct sockaddr_in myaddr, remaddr;
int sock, slen = sizeof(remaddr), recvlen;

int main(void){
    FILE *f;
    pthread_t threadRecv;
    
    char buf[BUFLEN];    /* message buffer */
    char server[IPLEN]; /* "123.206.52.64" "172.16.72.79"*/
    int service_port;

    if((f = fopen("client.log","a+"))==NULL)
        printf("Error when fopen\n");

    printf("Please input the ip of the server you want to connect:\n");
    scanf("%s",server);
    printf("Please input the port of the server you want to connect:\n");
    scanf("%d",&service_port);

    memset(&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family=AF_INET;
    myaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    myaddr.sin_port=htons(0);

    //Create the socket
    if((sock=socket(AF_INET, SOCK_DGRAM, 0))<0) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    if(bind(sock,( struct sockaddr *) &myaddr, sizeof(myaddr))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    memset((char *) &remaddr, 0, sizeof(remaddr));
    remaddr.sin_family = AF_INET;
    remaddr.sin_port = htons(service_port);
    if (inet_aton(server, &remaddr.sin_addr)==0) {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }
    // printf("Connect successfully!");
    time_t nowtime;
    nowtime = time(NULL);
    fprintf(f, "\nTime: %sConnected ip:port is %s:%d\n", ctime(&nowtime), server, service_port);
    fclose(f);

    pthread_create(&threadRecv, NULL, receiveMessage, NULL);

    while(1) {
        if((f = fopen("client.log","a+"))==NULL)
            printf("Error when fopen\n");
        fgets(buf, BUFLEN, stdin);
        fprintf(f, "%s", buf);
        if (sendto(sock, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, slen)==-1) {
            perror("sendto");
            exit(1);
        }
        /* now receive an acknowledgement from the server */
        fclose(f);
    }
   return 0;
}

void * receiveMessage(void * argument)
{
    int recvlen;
    FILE * f;
    char buf[BUFLEN];
    while(1)
    {
        if((f = fopen("client.log","a+"))==NULL)
            printf("Error when fopen\n");
        recvlen = recvfrom(sock, buf, BUFLEN, 0, (struct sockaddr *)&remaddr, (unsigned int*)&slen);
        if (recvlen >= 0) {
            buf[recvlen] = 0;   /* expect a printable string - terminate it */
            printf("received message: \"%s\"\n", buf);
            fprintf(f, "received message: \"%s\"\n", buf);
        }
        fclose(f);
    }

}

