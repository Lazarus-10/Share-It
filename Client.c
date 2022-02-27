
#include <stdio.h>
#include <stdlib.h>
#include<sys/time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <ctype.h>

#define portno 4444
#define file_name "fork.png"
// #define host_ip "127.0.0.1"
#define MILLION 1000000

void error(const char *msg){
    perror(msg);
    exit(1);
}



int main(int argc, char *argv[])
{
    freopen("ip_table.txt","r",stdin); // takes input from data.txt
    system("clear"); //clears the screen
    while (!feof(stdin)){ //checks for end of file
        int sockfd;
        struct sockaddr_in serv_addr;
        struct hostent *server; // Contains data-type for storing host information

        //create a new socket of type SOCK_STREAM(i.e. TCP) in domain AF_INET using default protocol
        sockfd = socket(AF_INET, SOCK_STREAM, 0);

        if (sockfd < 0)
            error("[-]Error opening socket");
        printf("[+]Opening Socket\n");

        char host_ip[256];
        scanf("%s", &host_ip[0]);
        printf("\tPinging %s .....\n",host_ip);
        sleep(1);

        server = gethostbyname(host_ip); //Return entry from host data base for host with NAME
        if (server == NULL){
            fprintf(stderr, "[-]Error, no such host\n\n");
            exit(0);
        }
        sleep(1);
        printf("[+]Node IP resolved: %s\n",host_ip);

        bzero((char *)&serv_addr, sizeof(serv_addr)); //erases the data in serv_addr variable

        //filling the required details of the server
        serv_addr.sin_family = AF_INET;
        bcopy((char *)server->h_addr_list[0], (char *)&serv_addr.sin_addr.s_addr, server->h_length);
        serv_addr.sin_port = htons(portno);

        //connecting to the server
        sleep(1);
        if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
            printf("[-]Connection Down\n");
            printf("----------------------------------------\n\n");
            goto end;
        }
        printf("[+]Connected to %s\n",host_ip);

        FILE *f = fopen(file_name, "rb"); //open the file to be transferred in read-binary mode
        if (f == NULL){
            printf("\n[-]%s not present",file_name);
            exit(1);
        }
        printf("[+]Payload Found\n");

        struct stat obj;

        stat(file_name, &obj);
        int file_size = obj.st_size; //get the file_size

        write(sockfd, &file_size, sizeof(file_size)); // send the file size to server
        printf("\nInitiating Payload .....\n\nFile size = %d Bytes\n",file_size);
        sleep(1);

        unsigned char buf[512]; // to store chunks of data from the target file
        int total_bytes_read = 0, bytes_written = 0, current_bytes_read = 0;

        struct timeval start, end;
        gettimeofday(&start, NULL); //Starting time

        while (total_bytes_read < file_size){ // stores the data in buf array

            current_bytes_read = fread(buf, 1, sizeof(buf), f);

            if (current_bytes_read > 0){
                bytes_written = write(sockfd, buf, current_bytes_read); // send the data stored in buf to Server
                total_bytes_read += bytes_written;
            }

            bzero(buf, 512); // clears the buf array
        }

        gettimeofday(&end, NULL); //Ending time

        double t1 = (end.tv_usec - start.tv_usec);                        //microseconds count
        double time_taken = (end.tv_sec - start.tv_sec) + (t1 / MILLION); // total time taken*/

        printf("\nPayload sent successfully at %s\n",host_ip);
        printf("----------------------------------------\n\n");

        fclose(f);     // close the file
        end: close(sockfd); //close the socket
    }

    return 0;
}


