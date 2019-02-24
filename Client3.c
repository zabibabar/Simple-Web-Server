//----- Include files ---------------------------------------------------------
#include <stdio.h>          // Needed for printf()
#include <string.h>         // Needed for memcpy() and strcpy()
#include <stdlib.h>         // Needed for exit()
#include <sys/types.h>    // Needed for sockets stuff
#include <netinet/in.h>   // Needed for sockets stuff
#include <sys/socket.h>   // Needed for sockets stuff
#include <arpa/inet.h>    // Needed for sockets stuff
#include <fcntl.h>        // Needed for sockets stuff
#include <netdb.h>        // Needed for sockets stuff

//----- Defines ---------------------------------------------------------------
#define  PORT_NUM		1050   // Port number used at the server
#define  IP_ADDR    	"127.0.0.1"  // IP address of server (*** HARDWIRED ***)
#define  BUFFER_SIZE	15

//===== Main program ==========================================================
int main()
{

	int                  client_s;        		// Client socket descriptor
	struct sockaddr_in   server_addr;     		// Server Internet address
	char                 out_buf[BUFFER_SIZE];  // Output buffer for data
	char                 in_buf[BUFFER_SIZE];   // Input buffer for data
	int                  retcode;        		// Return code

	// >>> Step #1 <<<
	// Create a client socket
	//   - AF_INET is Address Family Internet and SOCK_STREAM is streams
	client_s = socket(AF_INET, SOCK_STREAM, 0);
	if (client_s < 0)
	{
		printf("*** ERROR - socket() failed \n");
		exit(-1);
	}

	// >>> Step #2 <<<
	// Fill-in the server's address information and do a connect with the
	// listening server using the client socket - the connect() will block.
	server_addr.sin_family = AF_INET;                 // Address family to use
	server_addr.sin_port = htons(PORT_NUM);           // Port num to use
	server_addr.sin_addr.s_addr = inet_addr(IP_ADDR); // IP address to use
	retcode = connect(client_s, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (retcode < 0)
	{
		printf("*** ERROR - connect() failed \n");
		exit(-1);
	}

	// >>> Step #3 <<<
	// Send to the server using the client socket
	FILE *fp = fopen("Client3.txt", "r");
    int i=0;
    char c;
	while (fscanf(fp,"%c", &c) != EOF)  {
		out_buf[i] = c;
		i++;
	}
            
    fclose(fp);
  
	//strcpy(out_buf, "I am Client 1");
	// Output the sending message
	printf("Sending to server: %s \n", out_buf);
	retcode = send(client_s, out_buf, (strlen(out_buf) + 1), 0);
	if (retcode < 0)
	{
		printf("*** ERROR - send() failed \n");
		exit(-1);
	}

	// >>> Step #4 <<<
	// Receive from the server using the client socket
	retcode = recv(client_s, in_buf, sizeof(in_buf), 0);
	if (retcode < 0)
	{
		printf("*** ERROR - recv() failed \n");
		exit(-1);
	}

	// Output the received message
	printf("Received from server: %s \n", in_buf);
	
	// >>> Step #5 <<<
	// Close the client socket	
	retcode = close(client_s);
	if (retcode < 0)
	{
		printf("*** ERROR - close() failed \n");
		exit(-1);
	}
	
	// Return zero and terminate
	return(0);
}