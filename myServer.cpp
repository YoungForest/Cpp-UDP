#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#include <vector>

#define BUFSIZE 2048
#define MAXSOCKET 100

using namespace std;

void * run(void * arg);

int count = 0;

struct Client
{
    struct sockaddr *client_addr;
    int id;
    char name;
    struct sockaddr *dest;
    socklen_t addrlen;
    int fd;
};

int
main(int argc, char **argv)
{
    struct sockaddr_in myaddr;  /* our address */
    int recvlen;            /* # bytes received */
    int fd;             /* our socket */
    int msgcnt = 0;         /* count # of messages we received */
    unsigned char buf[BUFSIZE]; /* receive buffer */
    int service_port;
    vector<Client> clients;
    vector<int> sockets;

    printf("Which port do you want to bind?\n");
    scanf("%d", &service_port);
    
    /* bind the socket to any valid IP address and a specific port */
    memset((char *)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(service_port);

    for(int i=0; i<MAXSOCKET; i++)
    {
        if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("cannot create socket\n");
        return 0;
        }
        if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
            perror("bind failed");
            return 0;
        }
        sockets.push_back(fd);
    }
    /* now loop, receiving data and printing what we received */
    while(1) {
        struct sockaddr_in remaddr; /* remote address */
        socklen_t addrlen = sizeof(remaddr);        /* length of addresses */
        struct Client c;
        printf("waiting on port %d\n", service_port);
        fd = sockets[sockets.size()-1];
        sockets.pop_back();
        c.fd = fd;
        recvlen = recvfrom(fd, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen);
        c.client_addr = (struct sockaddr*)&remaddr;
        c.addrlen = addrlen;
        count++;
        c.id = count;
        pthread_t t;
        printf("Connecting one more...");

        pthread_create(&t, NULL, run, reinterpret_cast<void*>(&c));
    }
    /* never exits */
}

void * run(void *arg)
{
    struct Client *c = (struct Client *)arg;
    char buf[BUFSIZE];
    strcpy(buf, "Which person do you want to chat");
    if (sendto(c->fd, buf, strlen(buf), 0, (struct sockaddr *)&c->dest, c->addrlen) < 0)
        perror("sendto");
}