#include<stdio.h>
#include<string.h>    // for strlen
#include<arpa/inet.h> // for inet_addr
#include<unistd.h>    // for write
#include<time.h> //for time
#include<sys/socket.h>
#define PORT_NUMBER 60000

void writeTime(int newSocket , char format[]){  // this function provides --> time formatting
    char buf[256] = {0};
    time_t rawtime = time(NULL);  
    if (rawtime == -1) {   // if we have an error , it is shown on screen 
        puts("The time() function failed");
        return 1;
    }
    struct tm *ptm = localtime(&rawtime);
    if (ptm == NULL) {     // // if we have an error , it is shown on screen 
        puts("The localtime() function failed");
        return 1;
    }    
    puts(buf);   // puts our time buffer 
    strftime(buf, 256, format, ptm); // we format our time information
    puts(buf);   // puts our time buffer 
    memset(buf, 0, 256);   // we set our memory  
    strftime(buf, 256, format, ptm);      // we format our time information
    write(newSocket,"\n",strlen("\n"));    // new line command 
    write(newSocket,"SERVER RESPONSE : ",strlen("SERVER RESPONSE : "));   // to write server response
    write(newSocket,buf,strlen(buf));   // we write time info which telnet want 
    write(newSocket,"\n",strlen("\n"));   // new line command
}
int main(int argc, char *argv[])
{
    int socket_desc , new_socket , c;   // define sockets
    struct sockaddr_in server , client;   // we create server and client
    char *message;
    char error_message[20] = "INCORRECT REQUEST\n";  // our error message
    char received_message[1000];

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);   // we create first socket
    if (socket_desc == -1)    // if we cannot create first socket
    {
        puts("Could not create socket");
        return 1;
    }
    // we define our server features
    server.sin_family = AF_INET;   
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT_NUMBER);
    //Connect to remote server
    if(bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        puts("Binding failed");
        return 1;
    }
    puts("Socket is binded");
    listen(socket_desc, 3);  
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);   
    new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c);    // we create client server
    if (new_socket < 0)
    {
        puts("Accept failed");
        return 1;
    }
    puts("Connection accepted"); 
    while(1){
        int control = 1;  // control variable for get_time
        write(new_socket, "\nCLIENT REQUEST : ", strlen( "\nCLIENT REQUEST : "));  // client request  message
        read(new_socket,received_message,sizeof(received_message));   // read message from client
           
        if(strncmp(received_message,"GET_TIME_ZONE",13) == 0){  // if client request GET_TIME_ZONE
            writeTime(new_socket,"%z:"); // we call writeTime function for zone information
        }
        else if(strncmp(received_message,"GET_TIME_DATE",13) == 0){  // if client request GET_TIME_DATE
            writeTime(new_socket,"%T , %d. %m. %Y");   // we call writeTime function for time and date information
        }
        else if(strncmp(received_message,"GET_TIME",8) == 0 && control) {      // if client request GET_TIME
              writeTime(new_socket,"%T");   // we call writeTime function for time information
        }
        else if(strncmp(received_message,"GET_DATE",8) == 0){    // if client request GET_DATE
            writeTime(new_socket,"%d. %m. %Y");    // we call writeTime function for date information
        }
        else if(strncmp(received_message,"GET_DAY_OF_ALL_WEEK",19) == 0){  // if client request GET_DAY_OF_ALL_WEEK
           writeTime(new_socket,"%A");   // we call writeTime function for day information
            
        }else if(strncmp(received_message,"CLOSE_SERVER",12) == 0){  // if client request for CLOSE_SERVER
            write(new_socket, "\nGOODBYE\n", strlen( "\nGOODBYE\n"));  // we say "GOODBYE"
            close(socket_desc);  // we close server socket 
            close(new_socket);   // we close client socket
        }
        else
        {
                write(new_socket,"\n",strlen("\n"));
                write(new_socket, error_message, strlen(error_message));   // we write error message for incorrect request
        }  
    }
    return 0;
}