#include <stdio.h>	// Used for printf
#include <netinet/in.h>  // Used for socket stuff
#include <fcntl.h>	// Used for file IO
#include <unistd.h>	// Used for read and write
#include <errno.h>	// Used for perror
#include <stdlib.h>	// USed for exit
#include <strings.h>	// Used for bzero
#include <string.h>	// For strlen
#include <dirent.h>	// To walk through directories
#include <sys/stat.h>	// Used to check if file is directory

#define FILE_NAME_SIZE 100
#define BUFFER_SIZE	16
#define FILE_PATH_SIZE	100

// Print error message
void error(char* msg)
{
	perror(msg);
	exit(1);
}


// Seraches for file_name_to_search in current directory and sets file_path if found.
// Retuns 0 if  not found and 1 if found
int get_file_name(char* base_path, char* file_name_to_search, char* file_path)
{
	int found = 0;

	struct dirent* dp;
	DIR *dir = opendir(base_path);	// Open current directory
	if(dir == NULL)
		error("Could not open directory");

	while((dp = readdir(dir)) != NULL)
	{
		if(strcmp(dp->d_name, ".") && strcmp(dp->d_name, ".."))	// Check if dname is . or ..
		{
			char full_path_name[FILE_PATH_SIZE];	// Find path of current file
			strcpy(full_path_name, base_path);
			strcat(full_path_name, "/");
			strcat(full_path_name, dp->d_name);

			struct stat path_stat;
			stat(full_path_name, &path_stat);
			if(S_ISDIR(path_stat.st_mode)) // Check if file is a directory
			{
				found = get_file_name(full_path_name, file_name_to_search, file_path);
				if(found)
					break;
			}
			else
			{
				if(!strcmp(dp->d_name, file_name_to_search))	// Check if file found
				{
					strcpy(file_path, full_path_name);
					found = 1;
					break;
				}
			}
		}
	}

	closedir(dir);
	return found;
}

// Reads file_name and write to out_fd
void send_file(char* file_name, int out_fd)
{
	int fd;
	if((fd = open(file_name, O_RDONLY)) < 0)	// Open file for reading
		error("Could not open file");

	int bytes_read, bytes_write;
	char buffer[BUFFER_SIZE];
	while((bytes_read = read(fd, &buffer, BUFFER_SIZE-1)) > 0)	// Read till EOF
	{
		if((bytes_write = write(out_fd, &buffer, bytes_read)) < 0)
			error("Could not write");
		if(bytes_read != bytes_write)
			error("Error while transferring");
	}
	if(bytes_read < 0)
		error("Could not read");

	printf("Completed transfer!\n");

	close(fd);
}

int main(int argc, char const *argv[])
{

	// char file_base_path[FILE_PATH_SIZE] = ".";
	// char file_name_to_search[FILE_NAME_SIZE] = "here2";
	// char file_path[FILE_PATH_SIZE];
	// int found = get_file_name(file_base_path, file_name_to_search, file_path);
	// if(found)
	// 	printf("Found: %s\n", file_path);
	// else
	// 	printf("Not found\n");
	// return 0;

	if(argc < 2)	// Ensure port numbetr has been passed as an argument
		error("Usage ./server port(>2000)");

	int port = atoi(argv[1]);	// Fetch port to use
	printf("Using port %d\n", port);

	if(port < 2000)	// Ensure that an unreserved port number is specified
		error("Specify a higher port number (>2000)");

	struct sockaddr_in client_addr, server_addr;	//  Struct to store socket address data

	int sock_fd;	// File descriptor to access socket
	if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)	// Create socket and get a socket file descriptor
		error("Could not create socket");

	// Set fields in struct
	bzero((char*) &server_addr, sizeof(server_addr));	// Initialize struct to 0
	server_addr.sin_family = AF_INET;	// Family of protocols
	server_addr.sin_port = htons(port);	// Port number. Conver to network byte order by using htons
	server_addr.sin_addr.s_addr = INADDR_ANY;	// IP address on which server will be running. INADDR_ANY is a macro for IP of current machine

	if(bind(sock_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0)	// Bind socket to address specified in struct
		error("Unable to bind");

	listen(sock_fd, 0);	// Start listening for requests on socket. Don't have any backlog i.e. only ine client

	int client_addr_size = sizeof(client_addr);
	int client_sock_fd;
	int found = 0;

	if((client_sock_fd = accept(sock_fd, (struct sockaddr*) &client_addr, &client_addr_size)) < 0)	// Accept a new request and give client a new socket, client_sock_fd
		error("Could not accept requests");

	char buffer[FILE_NAME_SIZE];	// Buffer to read into
	bzero((char*) &buffer, sizeof(buffer));	// Initialize to 0
	if(read(client_sock_fd, &buffer, FILE_NAME_SIZE-1) < 0)	// Read data from client into buffer
		error("Unable to read from client");
	printf("Got the following file name from client: %s\n", buffer);

	char file_path[FILE_PATH_SIZE];
	char base_path[FILE_PATH_SIZE] = ".";	// Directory to start searching in
	found = get_file_name(base_path, buffer, file_path);	// Recursively search for file name in current directory
	if(found)
		send_file(file_path, client_sock_fd);	// Send file to client
	else
		printf("File not found!\n");

	close(sock_fd);
	close(client_sock_fd);
	return 0;
}