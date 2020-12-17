#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define BUF_SIZE 100000

/**
* Client code
* 1. Create a socket and connect to the server specified in the command arugments.
* 2. Prompt the user for input and send that input as a message to the server.
* 3. Print the message received from the server and exit the program.
*/

/*
convert character to interge
*/
int char_int(char letter){
      char operation[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
      int i;

      for(i = 0; i < 27; i++){
            if(operation[i] == letter){
                  break;
            }
      }
      i++;
      return i;
}


/*
Convert interge to character
*/
char int_chart(int index){
      char operation[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
      return operation[index - 1];
}


/*
decrypt plaintext into ciphertext
*/
void decrypte(char plaintext[], char key[]){
      int index;
      int i;
      for(i = 0; plaintext[i] != '\0'; i++){
            // decrypt plaintext into ciphertext
            index = char_int(plaintext[i]) - char_int(key[i]);
            // if a number is negative, then 27 is added to make the number zero or higher.
            if(index <= 0){
                  index = index + 27;
            }
            plaintext[i] = int_chart(index);
      }
      return;
}


// Error function used for reporting issues
void error(const char *msg) {
      perror(msg);
      exit(0);
}


// Set up the address struct
void setupAddressStruct(struct sockaddr_in* address, int portNumber){
      char* hostname = "localhost";
      // Clear out the address struct
      memset((char*) address, '\0', sizeof(*address));

      // The address should be network capable
      address->sin_family = AF_INET;
      // Store the port number
      address->sin_port = htons(portNumber);

      // Get the DNS entry for this host name
      struct hostent* hostInfo = gethostbyname(hostname);
      if (hostInfo == NULL) {
            fprintf(stderr, "CLIENT: ERROR, no such host\n");
            exit(0);
      }
      // Copy the first IP address from the DNS entry to sin_addr.s_addr
      memcpy((char*) &address->sin_addr.s_addr, hostInfo->h_addr_list[0], hostInfo->h_length);
      return;
}


/*
Check the key file or buffer if has a wrong character
*/
void check_bad_characters(char* text, int length){
      int i;
      char valid_chars[28] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ \n";

      for(i = 0; i < length; i++){
            if(strchr(valid_chars, text[i]) == NULL){
                  fprintf(stderr, "ERROR: key or plaintext file has bad characters.\n");
                  exit(1);
            }
      }
      return;
}


/*
Check if the length of key shorter than plaintext
*/
void check_key_length(int plaintext_length, int key_length){
      if(key_length < plaintext_length){
            fprintf(stderr, "ERROR: key file is shorter than the plaintext.\n");
            exit(1);
      }
      return;
}


int main(int argc, char *argv[]) {
      int socketFD, portNumber, charsWritten, charsRead;
      struct sockaddr_in serverAddress;
      char buffer[BUF_SIZE];
      char key_buffer[BUF_SIZE];

      // Check usage & args
      if(argc < 4){
            fprintf(stderr,"USAGE: %s hostname port\n", argv[0]);
            exit(0);
      }

      // Declare the file pointer
      FILE *filePointer;
      // Open the plaintext file using fopen()
      //printf("plaintext name: %s\n", plaintext_name);
      filePointer = fopen(argv[1], "r") ;

      // Check if this filePointer is null
      if (filePointer == NULL){
            fprintf(stderr, "ERROR: plaintext file failed to open.\n");
      }

      // Clear out the buffer
      memset(buffer, '\0', sizeof(buffer));
      // Read the content from the file
      // using fgets() method
      while(fgets(buffer, sizeof(buffer) - 1, filePointer));
      // Closing the file using fclose()
      fclose(filePointer) ;

      //printf("==buffer: %s\n", buffer);
      int plaintext_length = strlen(buffer);

      filePointer = fopen(argv[2], "r") ;

      // Check if this filePointer is null
      if (filePointer == NULL){
            fprintf(stderr, "ERROR: key file failed to open.\n");
      }

      // Clear out the key_buffer
      memset(key_buffer, '\0', sizeof(key_buffer));
      // Read the content from the file
      // using fgets() method
      while(fgets(key_buffer, sizeof(key_buffer) - 1, filePointer));
      // Closing the file using fclose()
      fclose(filePointer) ;

      // Get the length of the key
      int key_length = strlen(key_buffer);


      check_bad_characters(buffer, plaintext_length);
      check_bad_characters(key_buffer, key_length);
      check_key_length(plaintext_length, key_length);

      buffer[strcspn(buffer, "\n")] = '!';

      // Create a socket
      socketFD = socket(AF_INET, SOCK_STREAM, 0);
      if (socketFD < 0){
            fprintf(stderr, "CLIENT: ERROR opening socket.\n");
      }

      // Set up the server address struct
      setupAddressStruct(&serverAddress, atoi(argv[3]));

      // Connect to server
      if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
            fprintf(stderr, "CLIENT: ERROR connecting.\n");
      }

      // check to make sure it is communicating with enc_client
      charsWritten = send(socketFD, "enc", 3, 0);
      if (charsWritten < 0){
            fprintf(stderr, "CLIENT: ERROR writing to socket.\n");
      }
      if (charsWritten < 3){
            fprintf(stderr, "CLIENT: WARNING: Not all data written to socket!\n");
      }

      // check if connect to enc_server
      char check[1] = "1";
      charsWritten = recv(socketFD, check, 1, 0);
      if (charsWritten < 0){
            fprintf(stderr, "CLIENT: ERROR writing to socket.\n");
      }
      if(check[0] == '0'){
            fprintf(stderr, "CLIENT: Cannot connect to the enc_server.\n");
            exit(2);
      }

      // Send plaintext file to server
      // Write to the server
      int recv_length = 0;
      do{
            charsRead = send(socketFD, &buffer[recv_length], 1, 0);
            if (charsRead < 0){
                  error("ERROR writing to socket");
            }
            recv_length += charsRead;
      }while(buffer[recv_length-1] != '!');


      // Get return message from server
      // Clear out the buffer again for reuse
      memset(buffer, '\0', sizeof(buffer));
      key_buffer[strlen(key_buffer)] = '@';
      // Send key file to server
      // Write to the server
      recv_length = 0;
      do{
            charsRead = send(socketFD, &key_buffer[recv_length], 1, 0);
            if (charsRead < 0){
                  error("ERROR writing to socket");
            }
            recv_length += charsRead;
      }while(key_buffer[recv_length-1] != '@');

      // Get return message from server
      // Clear out the key buffer again for reuse
      //memset(key_buffer, '\0', sizeof(key_buffer));

      memset(buffer, '\0', sizeof(buffer));
      recv_length = 0;
      do{
            // Read data from the socket, leaving \0 at end
            charsRead = recv(socketFD, &buffer[recv_length], 1000, 0);
            if (charsRead < 0){
                  error("CLIENT: ERROR reading from socket");
            }
            recv_length += charsRead;
      }while(buffer[recv_length - 1] != '\0');

      printf("%s\n", buffer);

      // Close the socket
      close(socketFD);
      return 0;
}
