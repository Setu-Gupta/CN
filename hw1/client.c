#include <stdio.h>	// Used for printf
#include <netinet/in.h>  // Used for socket stuff
#include <unistd.h>	// Used for read and write
#include <errno.h>	// Used for perror
#include <stdlib.h>	// USed for exit
#include <strings.h>	// Used for bzero'
#include <string.h>	// For strlen
#include <netdb.h>	// For hostnet struct
#include <fcntl.h>	// For file IO

#define BUFFER_SIZE	16
#define FILE_NAME_SIZE	100

void error(char* msg)
{
	perror(msg);
	exit(1);
}

// Reads data from socket and writes to file_name
void read_from_server(int sock_fd, char* file_name)
{
	int out_fd;
	if((out_fd = open(file_name,  O_WRONLY | O_CREAT, 0644)) < 0)	// Open file to write to
		error("Could not open file to write to");

	char file_buffer[BUFFER_SIZE];
	int read_bytes, write_bytes;
	while((read_bytes = read(sock_fd, &file_buffer, BUFFER_SIZE-1)) > 0)	// Read data from client into buffer
	{
		if((write_bytes = write(out_fd, &file_buffer, read_bytes)) < 0 )
			error("Error while writing to file");
		if(write_bytes != read_bytes)
			error("Error while transferring");
	}

	printf("Completed transfer!\n");
	close(out_fd);
}

int main(int argc, char const *argv[])
{
	if(argc < 2)	// Ensure port numbetr has been passed as an argument
		error("Usage ./client host_name port(>2000");

	int port = atoi(argv[2]);	// Fetch port to use
	printf("Using port %d\n", port);

	if(port < 2000)	// Ensure that an unreserved port number is specified
		error("Specify a higher port number (>2000)");

	struct sockaddr_in server_addr;	//  Struct to store socket address data
	struct hostent* server;	// Pointer to hostnet struct

	int sock_fd;	// File descriptor to access socket
	if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)	// Create socket and get a socket file descriptor
		error("Could not create socket");

	if((server = gethostbyname(argv[1])) == NULL)
		error("No host by this name");

		// Set fields in struct
	bzero((char*) &server_addr, sizeof(server_addr));	// Initialize struct to 0
	server_addr.sin_family = AF_INET;	// Family of protocols
	server_addr.sin_port = htons(port);	// Port number. Conver to network byte order by using htons
	bcopy((char*)server->h_addr, (char*)&server_addr.sin_addr.s_addr, server->h_length);	// Save the IP addr found by gethostbyname

	if(connect(sock_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0)	// Bind socket to address specified in struct
		error("Unable to connect");

	char server_file_name[FILE_NAME_SIZE];	// Buffer to read file name into
	printf("Please enter file name to receive: ");
	scanf("%s", server_file_name);

	char out_file_name[FILE_NAME_SIZE];	// Buffer to read file name into
	printf("Please enter file name to output to: ");
	scanf("%s", out_file_name);

	if(write(sock_fd, &server_file_name, strlen(server_file_name)) < 0)	// Reply to server
		error("Could not write to server");
	
	read_from_server(sock_fd, out_file_name);

	close(sock_fd);
	return 0;
}