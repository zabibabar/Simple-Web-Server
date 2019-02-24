//----- Include files ---------------------------------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>

//----- Defines ---------------------------------------------------------------
#define  PORT_NUM   1050    // Arbitrary port number for the server
#define BUFFER_SIZE 15

//----- Global Variables-------------------------------------------------------
char message[BUFFER_SIZE];          // buffer for data shared between server and client
sem_t mutex;                        // the semaphore to protect shared memory
void* ServerHandler (void* arg);    // thread function to handle different clients

//===== Main program ==========================================================
int main()
{
    int                  welcome_s;       // Welcome socket descriptor
    struct sockaddr_in   server_addr;     // Server Internet address
    int                  connect_s;       // Connection socket descriptor
    struct sockaddr_in   client_addr;     // Client Internet address
    struct in_addr       client_ip_addr;  // Client IP address
    int                  addr_len;        // Internet address length
    int                  retcode;         // Return code

    pthread_t	tid1[1];          /* process id for thread 1 */
    pthread_t   tid2[1];          /* process id for thread 2 */
    pthread_t   tid3[1];          /* process id for thread 3 */
    pthread_attr_t	attr[1];      /* attribute pointer array */
    sem_init(&mutex, 0, 1);       /* Create the mutex lock and initialize to 1*/

    /* Required to schedule thread independently.*/
    pthread_attr_init(&attr[0]);
    pthread_attr_setscope(&attr[0], PTHREAD_SCOPE_SYSTEM);
    /* end to schedule thread independently */

    // Create a welcome socket
    //   - AF_INET is Address Family Internet and SOCK_STREAM is streams
    welcome_s = socket(AF_INET, SOCK_STREAM, 0);
    if (welcome_s < 0)
    {
        printf("*** ERROR - socket() failed \n");
        exit(-1);
    }

    // Fill-in server (my) address information and bind the welcome socket
    server_addr.sin_family = AF_INET;                 // Address family to use
    server_addr.sin_port = htons(PORT_NUM);           // Port number to use
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Listen on any IP address
    retcode = bind(welcome_s, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (retcode < 0)
    {
        printf("*** ERROR - bind() failed \n");
        exit(-1);
    }
    int num_connections = 0;
    while (num_connections < 3)
    {
        // Listen on welcome socket for a connection
        listen(welcome_s, 1);

        // Accept a connection.  The accept() will block and then return with
        // connect_s assigned and client_addr filled-in.
        //printf("Waiting for accept() to complete... \n");
        addr_len = sizeof(client_addr);
        connect_s = accept(welcome_s, (struct sockaddr *)&client_addr, &addr_len);
        if (connect_s < 0)
        {
            printf("*** ERROR - accept() failed \n");
            exit(-1);
        }
        
        

        // Copy the four-byte client IP address into an IP address structure
        memcpy(&client_ip_addr, &client_addr.sin_addr.s_addr, 4);

        //printf("Accept completed (IP address of client = %s  port = %d) \n",
        //inet_ntoa(client_ip_addr), ntohs(client_addr.sin_port));

    	num_connections++;

        int *newSock = malloc(1);
        *newSock = connect_s;

        if (num_connections == 1)
            pthread_create(&tid1[0], &attr[0], ServerHandler, (void*) newSock);
        else if (num_connections == 2)
            pthread_create(&tid2[0], &attr[0], ServerHandler, (void*) newSock);
        else if (num_connections == 3)
            pthread_create(&tid3[0], &attr[0], ServerHandler, (void*) newSock);
    }

    // Wait for the threads to finish 
    pthread_join(tid1[0], NULL);
    pthread_join(tid2[0], NULL);
    pthread_join(tid3[0], NULL);

    /* Destroying semaphores*/
    sem_destroy(&mutex);

    /*Terminate threads */
    pthread_exit(NULL);

    // Close the welcome and connect sockets
    retcode = close(welcome_s);
    if (retcode < 0)
    {
        printf("*** ERROR - close() failed \n");
        exit(-1);
    }

    retcode = close(connect_s);
    if (retcode < 0)
    {
        printf("*** ERROR - close() failed \n");
        exit(-1);
    }
    
    return(0);
}

void* ServerHandler (void* arg)
{
    int retcode;
    int connect_s = *(int*) arg;

    sem_wait(&mutex);

    // Receive from the client using the connect socket
    retcode = recv(connect_s, message, sizeof(message), 0);
    if (retcode < 0)
    {
        printf("*** ERROR - recv() failed \n");
        exit(-1);
    }
    printf("Received from client: %s \n", message);

    sleep(2);
    
    // Send to the client using the connect socket
    retcode = send(connect_s, message, sizeof(message), 0);
    if (retcode < 0)
    {
        printf("*** ERROR - send() failed \n");
        exit(-1);
    }

    sem_post(&mutex);
}
