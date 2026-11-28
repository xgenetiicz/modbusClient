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
#include <stdlib.h> // for perror

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

    
    //I need a perror to check the status
    if(status < 0) {
        perror("TCP Connection failed");
        exit(EXIT_FAILURE);
    }

    /*So here will i create the request for the different registers for the modbus protocol.
    Fetching from the server (ESP32) where this is a slave that holds three registries such as temperature,humidity and uptime.
    */
    
    //https://simplymodbus.ca/learn-tcp.html
    
    //REMEMBER -> EACH REGISTER IS TWO BYTES = 16-BIT. but i need two arrays, this is first to build the 12 byte array.
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
    //*DONE*
    send(client_fd,request,sizeof(request),0);
    printf("Sending Modbus Request..\n");

    //Now i need a for loop to print all the values and the hexadecimals from the responseBuffer
    valread = read(client_fd,responseBuffer,sizeof(responseBuffer));

    printf("Modbus buffer: \n");
    for (int i = 0; i < valread; i++) { // valread has stored the read of client, responsebuffer, and the size of the array
        printf("%02X ",responseBuffer[i]); //print the whole array with spacing 
    }
    printf("\n");

    /*
    Now i need to print the values into correct order as 16-bit.
    I don't want to lose any values, but i want to get the bits that represents 1.
    so therefore i should use | OR operator where this will check if the bits are represented as for example
        0000 0101 //0x05
        1111 0010 //0xF2
    r = 1111 0111 //0xF7 for example
    So the modbus protocol always use the big endian first! the high byte will always come first, and then the low byte will come later.
    */ 

    /*  Sending Modbus Request..
        Modbus buffer:
        00 01 00 00 00 0B 01 03 08 04 D2 00 00 00 D8 02 06

        This is the result i got from the program, now the mbap header is 7 bytes. i = 0 -> 00 | i = 6 -> 01. So these are 7 elements = 7 bytes, each group of two hex digits are one byte. 
        00 01 00 00 00 0B 01 -> MBAP HEADER (modbus application header) 7 bytes
        the function code is i=7-> 03. 1 byte
        the data address for the protocol is at element i = 8-> 08. This is a byte count, that counts databytes which will follow. I have 4 registers, so 2 bytes x 4 registers are 8 bytes.
            AND FROM i = 9->04 are the 4 registries right from the esp32. 04D2=testvalue,0000=uptime, 00D8=temperature, 0206=humidity right.

         so the response in total then have 7 (mbap header) + 1(function code) + 1 (byte count) + 8 bytes from the different registries = 17 bytes in total.
    */
    uint16_t testValue =(responseBuffer[9] << 8 | responseBuffer[10]); // high byte first (big-endian): shift it 8 bits to the left, then OR in the low byte, so both end up in one 16 bit value. OR for checking if there are any 1's
    int16_t upTime =(responseBuffer[11] << 8 | responseBuffer[12]); //the uptime, but right now they are just 00 00; I haven't wrote the code in the arduino
    int16_t temperature = (responseBuffer[13] << 8) | responseBuffer[14]; // 00 D8, and this as signed since the temperature can have negative values.
    uint16_t humidity = (responseBuffer[15] << 8 | responseBuffer[16]); // 02 06, and this is unsigned since humidity cannot be negative.

    printf("%u\n",testValue);
    printf("%u\n",upTime);
    printf("%.1f\n",temperature / 10.0);
    printf("%.1f\n",humidity / 10.0);

    //closing the connected socket
    close(client_fd);
    return 0;
}