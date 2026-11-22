/*
So i will create a main.c to test it with:

 gcc -Wall -Wextra -o modbus main.c  // this way is to use the compiler and test it then later with on wsl with ./modbus
 modbus is the actual program that takes the arguments
*/

/*
Okay, i have some libraries, but I will maybe remove some later, idk how to solve this
so im trying to find out.

https://www.geeksforgeeks.org/c/socket-programming-cc/ - lot of information about how i should implement it.
*/

#include <stdint.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 502 // SO THE MODBUS PORT BY DEFAULT IS 502.

int main(int argc, char *argv[]){

    int status,valread,client_fd;
    struct sockaddr_in addr;
    char *hello = "Hello from Client!"; // TODO: This should be an unsigned 8 int requests[12] an array of 12 elements, and with this array i should requests the data from the esp32
    char buffer[1024] = { 0 };


    /*Here do i create a check if arguments is more than two, it should respond to the user that there are too many arguments in program.
    It should only take one thing as an argument, and that is exactly the ip address of the esp32 server!
    ./<programname> is always one argument, the first one as argv[0].
    */

    if(argc > 2) { //im saying if argument count
        printf("This Modbus program takes only two arguments. <programname> & <IP-ADDRESS> you are looking for.\n"); //if i run the program with ./modbus <ip-address> & <something else> won't run.
        return -1;
    }

    if(argc < 2) { //im saying if argument count
        printf("This Modbus program takes an <IP-ADDRESS> as an argument, please provide the address to the Modbus Server.\n"); // If i just run the program with ./modbus -> i will get this message to inform.
        return -1;
    }

    //So this initalise the clientfd variable that holds the socket
    //AF_INET is the address family to specify ipv4 addresses. 
    //Sock stream is for reliable connection oriented tcp.
    if((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&addr,0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT); // host to network, and the port should be 502 because of modbus.
    if (inet_pton(AF_INET, argv[1], &addr.sin_addr) <= 0){ // point it to the addresss of the socket->EDIT: PASSED THE first argument to the program, where this should be the ip adress of the esp32!
        printf("\n Invalid address/ Address not supported. \n");
        return -1; // return -1 stops the program.
    }
    
    /*inet_pton is a presentation to network where this converts IPv4 or IPv6 address string 
    into binary form in network byte order.
    */


    status = connect(client_fd, (struct sockaddr *) &addr,sizeof(addr));

    if(status < 0) {
        printf("\n Connection Failed \n");
        return -1;
    }

    //subtract 1 for the null
    send(client_fd,hello,strlen(hello),0);
    printf("\n Hello message sent!\n");
    valread = read(client_fd,buffer,1024-1);
    printf("%s\n",buffer);
    
    //closing the connected socket
    close(client_fd);
    return 0;
}