#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>

#define portno 4444
#define PBSTR "|||||||||||||||||||||||||||||||||||"
#define file_name "Payload.png"
#define PBWIDTH 35

void printProgress(double percentage)
{
    int val = (int)(percentage * 100);
    int lpad = (int)(percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush(stdout);
}

void error(const char *msg){
    perror(msg);
    exit(1);
}

void print_ls(const char *msg){
    system("clear");
    printf("\t\t   %s\n\n",msg);
    system("ls>ls.txt"); // put the list of all file names in ls.txt

    printf("---------------------------------------------------------\n");
    FILE *ls_ptr = fopen("ls.txt","r"); //opening ls.txt in read-only mode
    char c;
    while((c = getc(ls_ptr)) != EOF){
        fputc(c,stdout); //prints the content of ls.txt in output stream
    }
    fclose(ls_ptr); //close the file after printing
    printf("---------------------------------------------------------\n");
    remove("ls.txt");
}

int main(int argc, char *argv[])
{   

    system("clear");  //clears the screen
    printf("Server Started......");
    printf("\n\nWaiting for Client to connect.........");
    fflush(stdout);

    int sockfd, newsockfd;
    socklen_t clilen;
    // char file_name[512];
    struct sockaddr_in serv_addr, cli_addr;


    //create a new socket of type SOCK_STREAM(i.e. TCP) in domain AF_INET using default protocol
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    //filling the required details of the server
    bzero((char *) &serv_addr, sizeof(serv_addr)); //clears any kind of data in serv_addr variable

 //  The htons function can be used to convert an IP port number in host byte order to the IP port number in network byte order.
    serv_addr.sin_port = htons(portno); // http://bit.ly/3lNaMaN

    //   AF_INET is an address family that is used to designate the type of addresses that our socket can communicate with(here IPv4)
    serv_addr.sin_family = AF_INET; // http://bit.ly/3gdGyws
    
    //   It will bind socket to all available interfaces(not just localhost)
    serv_addr.sin_addr.s_addr = INADDR_ANY; // http://bit.ly/2JwUIwW

   

    //  Binding the server information
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        printf("port %d in use\n",ntohs(serv_addr.sin_port));//error("ERROR on binding");
    else
        printf("\n\nPORT number used is %d\n",ntohs(serv_addr.sin_port));
     
    // listening for connections
    listen(sockfd,5); //backlog 5

    clilen = sizeof(cli_addr);

    //accepting the incoming connection and generating a new socket file descriptor
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) 
        error("ERROR on accept");
    
    print_ls("Initital list of files");
    fflush(stdout);

    // reading file size from client
    int file_size;
    read(newsockfd,&file_size,sizeof(file_size));

    FILE *fp;
    fp = fopen(file_name,"wb");  // open corresponding file in write-binary mode
 
    unsigned char buf[512]; // to store a chunk of data coming from client file
    int total_bytes_received = 0, current_bytes_received = 0;
    
    system("clear");
    printf("File size = %d Bytes\n\n",file_size);
    printf("Receiving \"%s\" .....\n",file_name);
    fflush(stdout);
    double percentage = 0.0;

    while(total_bytes_received < file_size){        
        bzero(buf, 512); // clears the buf array
        current_bytes_received = read(newsockfd, buf, sizeof(buf)); // read the corresponding data coming from client to buf array
        fwrite(buf, 1, current_bytes_received, fp); // write the corresponding data from buf array to file
        total_bytes_received += current_bytes_received;
        printProgress(((double)total_bytes_received)/file_size); 
        
    
    }
    
    fseek(fp, 0, SEEK_SET); //set file pointer at beginning
    
    printf("\n\n%s received successfully\n",file_name);
    sleep(1);
    print_ls("Final list of files");
    system("eog Payload.png");

    fclose(fp); // close the file
    close(sockfd); // close server socket
    close(newsockfd); //close interconnected socket
  
    return 0; 
    
}
