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

char *semName = "/SEM";
int count = 0;
char buf[BUFSIZE]; /* receive buffer */
struct sockaddr_in remaddr; /* remote address */

struct Client
{
    struct sockaddr_in client_addr;
    int id;
    char name[BUFSIZE];
    char password[BUFSIZE];
    int birthyear;
    int birthmonth;
    int birthday;
    struct Client *dest;
    socklen_t addrlen;
    int fd;
    int ifMess;
};

void m_threadSleep(int sec,int nsec)
{
    struct timespec sleepTime;
    struct timespec returnTime;
    sleepTime.tv_sec = sec;
    sleepTime.tv_nsec = nsec;
    nanosleep(&sleepTime, &returnTime);
}

int
main(int argc, char **argv)
{
    FILE *f;
    struct sockaddr_in myaddr;  /* our address */
    int recvlen;            /* # bytes received */
    int fd;             /* our socket */
    int msgcnt = 0;         /* count # of messages we received */
    // unsigned 
    int service_port;
    vector<Client> clients;

    if((f = fopen("chat.log","a+"))==NULL)
        printf("Error when fopen\n");

    printf("Which port do you want to bind?\n");
    scanf("%d", &service_port);

    fprintf(f, "Listening to port %d\n", service_port);
    
    memset((char *)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(service_port);

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("cannot create socket\n");
        return 0;
    }
    if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
        perror("bind failed");
        return 0;
    }
    struct sockaddr_in remaddr; /* remote address */
    socklen_t addrlen = sizeof(remaddr);        /* length of addresses */
    while(1) {
        socklen_t addrlen = sizeof(remaddr);        /* length of addresses */
        printf("waiting on port %d\n", service_port);
        
        recvlen = recvfrom(fd, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen);
        sprintf(buf, "User-%d : %s", clients[i].id, buf);
        fprintf(f, "%s", buf);
        int i;
        for(i=0; i<clients.size(); i++) {
            if(remaddr.sin_addr.s_addr == clients[i].client_addr.sin_addr.s_addr) 
            {
                // printf("equal\n");
                for (int j=0; j<clients.size(); j++)
                {
                    clients[j].ifMess = 1;
                }
                break;
            }
            else
            {
                struct Client *c = &clients[i];
                if (sendto(c->fd, buf, strlen(buf), 0, (struct sockaddr *)&c->client_addr, c->addrlen) < 0)
                    perror("sendto");
            }
        }
        if(i == clients.size()) {
            struct Client c;
            c.fd = fd;
            c.client_addr = remaddr;
            c.addrlen = addrlen;
            count++;
            c.id = count;
            c.ifMess = 0;
            strcpy(c.name, "unamed");
            clients.push_back(c);
            pthread_t t;
            printf("A new client connects.\n");
            pthread_create(&t, NULL, run, &c);
            pthread_join(t, NULL);
        }
    }
    /* never exits */
}

void * run(void *arg)
{
    struct Client *c = (struct Client *)arg;
    char client_buf[BUFSIZE];
    strcpy(client_buf, "Hi, new guy! Welcome to our chat room!");
    printf("%s\n", client_buf);
    if (sendto(c->fd, client_buf, strlen(client_buf), 0, (struct sockaddr *)&c->client_addr, c->addrlen) < 0)
        perror("sendto");
}