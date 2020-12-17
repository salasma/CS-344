#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define BUF_SIZE 100000


// Error function used for reporting issues
void error(const char *msg) {
      perror(msg);
      exit(1);
}

// Set up the address struct for the server socket
void setupAddressStruct(struct sockaddr_in* address, int portNumber){
      // Clear out the address struct
      memset((char*) address, '\0', sizeof(*address));

      // The address should be network capable
      address->sin_family = AF_INET;
      // Store the port number
      address->sin_port = htons(portNumber);
      // Allow a client at any address to connect to this server
      address->sin_addr.s_addr = INADDR_ANY;
}


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
convert interge to character
*/
char int_chart(int index){
      char operation[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
      return operation[index - 1];
}


/*
Get the information
*/
void get_client_information(char buffer[], char key[], char plaintext[]){
      int i, j;
      // Get the information of plaintext
      for(i = 0; buffer[i] != '\0'; i++){
            plaintext[i] = buffer[i];
      }
      // Set the end pointer
      plaintext[i] = '\0';
      i++;

      // Get the information of key
      for(j = 0; buffer[i + j] != '\0'; j++){
            key[i] = buffer[i + j];
      }
      // Set the end pointer
      key[i] = '\0';

      return;
}


/*
encrypt plaintext into ciphertext
*/
void encrypte(char plaintext[], char key[]){
      int index;
      int i;
      for(i = 0; plaintext[i] != '\0'; i++){
            index = char_int(plaintext[i]) - char_int(key[i]);
            //printf("%d %d ",char_int(plaintext[i]), char_int(key[i]) );
            if(index < 1){
                  index = index + 27;
            }
            //printf("%d\n", index );
            plaintext[i] = int_chart(index);
            //printf("%d\n",plaintext[i] );
      }

      return;
}


int main(int argc, char *argv[]){
      int connectionSocket, charsRead;
      char buffer[BUF_SIZE];
      char plaintext[BUF_SIZE];
      char key[BUF_SIZE];
      int childStatus;
      int num_of_porc = 0;
      struct sockaddr_in serverAddress, clientAddress;
      socklen_t sizeOfClientInfo = sizeof(clientAddress);

      // Check usage & args
      if (argc < 2) {
            fprintf(stderr,"USAGE: %s port\n", argv[0]);
            exit(1);
      }

      // Create the socket that will listen for connections
      int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
      if (listenSocket < 0) {
            error("ERROR opening socket");
      }

      // Set up the address struct for the server socket
      setupAddressStruct(&serverAddress, atoi(argv[1]));

      // Associate the socket to the port
      if (bind(listenSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0){
            error("ERROR on binding");
      }

      // Start listening for connetions. Allow up to 5 connections to queue up
      listen(listenSocket, 5);

      // Accept a connection, blocking if one is not available until one connects
      while(1){
            // Accept the connection request which creates a connection socket
            connectionSocket = accept(listenSocket, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);

            if (connectionSocket < 0){
                  error("ERROR on accept");
            }

            // support up to five concurrent socket connections running at the same time
            pid_t childPid = waitpid(-1, &childStatus, WNOHANG);
            if(childPid > 0){
                  num_of_porc --;
            }

            // support up to five concurrent socket connections running at the same time
            if(num_of_porc >= 5){
                  printf("There are more than 5 concurrent socket conections running at the same time.\n");
                  while(num_of_porc >= 5){
                        childPid = waitpid(-1, &childStatus, WNOHANG);
                        if(childPid > 0){
                              num_of_porc --;
                        }
                  }
            }


            // fork a process
            pid_t spawPid = fork();

            if(spawPid == -1){
                  fprintf(stderr, "fork() failed!\n");
            }
            else if(spawPid == 0){
                  memset(buffer, '\0', BUF_SIZE);
                  do{
                        // Get the message from the client and display it
                        // Read the client's message from the socket
                        charsRead = recv(connectionSocket, buffer, 3, 0);
                        if (charsRead < 0){
                              error("ERROR reading from socket");
                        }
                        //printf("SERVER: I received this from the client: \"%s\"\n", buffer);
                  }while(buffer[2] != 'c');

                  // check to make sure it is communicating with enc_client
                  if(buffer[0] != 'd' || buffer[1] != 'e' || buffer[2] != 'c'){
                        fprintf(stderr, "ERROR: It is not communicate with enc_client.\n");
                        // Send a failed message back to the client
                        charsRead = send(connectionSocket, "0", 1, 0);
                        if (charsRead < 0){
                              error("ERROR writing to socket");
                        }
                        exit(2);
                  }
                  else{
                        // successful, and return
                        charsRead = send(connectionSocket, "1", 1, 0);
                        if (charsRead < 0){
                              error("ERROR writing to socket");
                        }
                  }

                  // Get the message from the client and display it
                  memset(buffer, '\0', BUF_SIZE);
                  int recv_length = 0;
                  do{
                        // Read the client's message from the socket
                        charsRead = recv(connectionSocket, &buffer[recv_length], 1, 0);
                        if (charsRead < 0){
                              error("ERROR reading from socket");
                        }
                        recv_length += charsRead;
                        //printf("SERVER: I received this from the client: \"%s\"\n", buffer);
                  }while(buffer[recv_length - 1] != '!');
                  buffer[recv_length-1] = '\0';
                  //printf("%s\n",buffer );

                  memset(key, '\0', BUF_SIZE);
                  recv_length = 0;
                  do{
                        // Read the client's message from the socket
                        charsRead = recv(connectionSocket, &key[recv_length], 1, 0);
                        if (charsRead < 0){
                              error("ERROR reading from socket");
                        }
                        recv_length += charsRead;
                        //printf("SERVER: I received this from the client: \"%s\"\n", buffer);
                  }while(key[recv_length - 1] != '@');
                  //get_client_information(buffer, key, plaintext);
                  key[recv_length -1 ] = '\0';

                  encrypte(buffer, key);

                  recv_length = 0;
                  do{
                        charsRead = send(connectionSocket, &buffer[recv_length], 1, 0);
                        if (charsRead < 0){
                              error("ERROR writing to socket");
                        }
                        recv_length += charsRead;
                  }while(buffer[recv_length-1] != '\0');

                  // Close the connection socket for this client
                  close(connectionSocket);
                  exit(0);
            }
            else{
                  num_of_porc ++;
                  childPid = waitpid(-1, &childStatus, WNOHANG);
                  if(childPid > 0){
                        num_of_porc --;
                  }
            }
      }
      // Close the listening socket
      close(listenSocket);
      return 0;
}
