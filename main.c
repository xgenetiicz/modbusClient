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
    uint8_t responseBuffer[1024] = { 0 }; //so this responseBuffer should retrieve the values from the request


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

    /*So here will i create the request for the different registers for the modbus protocol.
    Fetching from the server (ESP32) where this is a slave that holds three registries such as temperature,humidity and uptime.
    */
    
    //https://simplymodbus.ca/learn-tcp.html
    
    //REMEMBER -> EACH REGISTER IS TWO BYTES = 16-BIT. but i need two arrays, this is first to build the 12 byte array, and later on i will have a uint8_t response[12] that get the responses and declare
    //them in uint16 and int16
    uint8_t request[12] = { //MODBUS MBAP HEADER & MODBUS TCP/IP PDU
        0x00,0x01, // transaction Id
        0x00,0x00, // protocol id
        0x00,0x06, // message length
        0x01, // unit identifier
        0x03, //Function code -> where this read my registries from the esp32 -> temperature, humidity and uptime and testvalue. So i have 4 registers. each function code has its own table. coils,disrete inputs, holding registers that are the addHreg from esp32. And also input registers that i don't need.
        0x00,0x00, // the data address for the first register requested
        0x00,0x04 // total number of registers requested
    };

    //TODO: send() and recv() and then a for loop of the array of responseBuffer and the print the values in hex.
    send(client_fd,request,sizeof(request),0);
    printf("\n Sent Modbus Request\n");

    //Now i need a for loop to print all the values and the hexadecimals from the responseBuffer
    valread = read(client_fd,responseBuffer,sizeof(responseBuffer));

    for (int i = 0; i < valread; i++) { // valread has stored the read of client, responsebuffer, and the size of the array
        printf("%02X ",responseBuffer[i]); //print the whole array
    }

    printf("\n");

    //closing the connected socket
    close(client_fd);
    return 0;
}