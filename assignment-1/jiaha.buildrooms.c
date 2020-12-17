#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


struct Room{
      char* name;
      char* type;
      int numOutBoundConnections;
      struct Room* outBoundConnections[6];
};


/*
create a struct room
*/
struct Room* create_room(){
      struct Room* room = malloc(7 * sizeof(struct Room));

      int i;
      int j;
      for(i = 0; i < 7; i++){
            room[i].name = NULL;
            room[i].type = NULL;
            room[i].numOutBoundConnections = 0;
            for(j = 0; j < 6; j++){
                  room[i].outBoundConnections[j] = NULL;
            }
      }

      return room;
}


/*
create a directory
*/
int create_directory(){
      char dir_name[20] = "jiaha.rooms.";
      char str_rand_num[10];

      // Get a random number between 0 and 100000
      int rand_num = rand() % 100000 + 1;

      // Convert int to string
      sprintf(str_rand_num, "%d", rand_num);
      // Appends the string pointed to by "str_rand_num" to the end of the string pointed to by "dir_name".
      strcat(dir_name, str_rand_num);

      // Create a directory named "jiaha.rooms.random_number"
      int result = mkdir(dir_name, 0755);

      // If creat a directory fail, print error message and exit program
      if(result != 0){
            printf("Fail to open a directory\n");
            exit(0);
      }

      return rand_num;
}


/*
Set the array to -1, in order to check
*/
void set_temp_array(int temp_arr[]){
      int i;

      // Set array to -1
      for(i = 0 ; i < 7; i++){
            temp_arr[i] = -1;
      }
}


/*
Check the random number if repeat
*/
bool check_repeat(int num, int temp_arr[]){
      int i;

      // Check temp array if has a num equal this random number
      for(i = 0; i < 7; i++){
            if(temp_arr[i] == num){
                  return true;
            }
      }

      // If get a non-repeat random number, insert this number to temp array
      // all number remove one index to tail
      for(i = 6; i >= 1; i--){
            temp_arr[i] = temp_arr[i-1];
      }
      // Set the first index to random number
      temp_arr[0] = num;
      return false;
}


/*
Randomly get the room name
*/
void get_room_name(char* room_name[]){
      int i = 0;
      int rand_num = 0;
      int temp_arr[7];

      // Set the temp array to -1
      set_temp_array(temp_arr);

      // This is room name inventory
      char* roomName[] = {"Sweet", "Gasa", "Louvre", "Imperial", "Sagrada",
                          "Eiffel", "Grand", "Golden", "Notre", "Dame"};

      // Randomly get 7 room names
      for(i = 0; i < 7; i++){
            // check repeat
            do{
                  rand_num = rand() % 10;
            } while(check_repeat(rand_num, temp_arr));

            // Set room name
            room_name[i] = roomName[rand_num];
      }
      return;
}


/*
Randomly get room type
*/
void get_room_type(char* room_type[]){
      // This is room typee inventory
      char* type[] = {"START_ROOM", "MID_ROOM", "MID_ROOM", "MID_ROOM",
                      "MID_ROOM", "MID_ROOM", "END_ROOM"};
      int i = 0;
      int rand_num = 0;
      int temp_arr[7];

      for(i = 0; i < 7; i++){
            // check repeat
            do{
                  rand_num = rand() % 7;
            }while(check_repeat(rand_num, temp_arr));

            room_type[i] = type[rand_num];
      }
}


/*
Set room name and type
*/
void set_room_information(struct Room* room, char* room_name[], char* room_type[]){
      int i;

      for(i = 0; i < 7; i++){
            room[i].name = room_name[i];
            room[i].type = room_type[i];
      }
      return;
}


/*
Returns true if all rooms have 3 to 6 outbound connections, false otherwise
*/
bool IsGraphFull(struct Room* room){
      int i;

      // Check if the number of room's connections less than 3
      for(i = 0; i < 7; i++){
            if(room[i].numOutBoundConnections < 3)
                  return false;
      }
      return true;
}


/*
Returns a random Room, does NOT validate if connection can be added
*/
struct Room* GetRandomRoom(struct Room* room){
      int temp = rand() % 7;

      return &room[temp];
}


/*
Returns true if a connection can be added from Room x (< 6 outbound connections), false otherwise
*/
bool CanAddConnectionFrom(struct Room* x){
      if(x->numOutBoundConnections < 6)
            return true;
      else
            return false;
}


/*
Returns true if Rooms x and y are the same Room, false otherwise
*/
bool IsSameRoom(struct Room* x, struct Room* y){
      if(x->name == y->name)
            return true;
      else
            return false;
}


/*
Returns true if a connection from Room x to Room y already exists, false otherwise
*/
bool ConnectionAlreadyExists(struct Room* x, struct Room* y){
      int i;

      // Check if a connection from Room x to Room y already exists
      for(i = 0; i < x->numOutBoundConnections; i++){
            if(x->outBoundConnections[i]->name == y->name)
                  return true;
      }
      return false;
}


/*
Connects Rooms x and y together, does not check if this connection is valid
*/
void ConnectRoom(struct Room* x, struct Room* y){
      // Connects Rooms x to y
      x->outBoundConnections[x->numOutBoundConnections] = y;
      x->numOutBoundConnections += 1;

      // Connects Rooms y to x
      y->outBoundConnections[y->numOutBoundConnections] = x;
      y->numOutBoundConnections += 1;

      return;
}


/*
Adds a random, valid outbound connection from a Room to another Room
*/
void AddRandomConnection(struct Room* room){
      struct Room* A;
      struct Room* B;

      // Maybe a struct, maybe global arrays of ints
      // Randomly get a room
      do{
            A = GetRandomRoom(room);
      }while(CanAddConnectionFrom(A) == false);

      // Randomly get a room
      // Check if room B is not room A and connection is not already exists
      do{
            B = GetRandomRoom(room);
      }while(CanAddConnectionFrom(B) == false || IsSameRoom(A, B) == true || ConnectionAlreadyExists(A, B) == true);

      // TODO: Add this connection to the real variables,
      ConnectRoom(A, B);

      return;
}


/*
Create all connections in graph
*/
void create_all_connections(struct Room* room){
      // Check if the number of room's connections less than 6
      while (IsGraphFull(room) == false){
            AddRandomConnection(room);
      }

      return;
}


/*
Create files in room directory, and write room information into file
*/
void create_file_in_directory(struct Room* room, int rand_num){
      int i;
      int j;
      char pathName[50];

      // Create a file
      FILE *fp;

      // Print room information to each file
      for(i = 0; i < 7; i++){
            sprintf(pathName, "./jiaha.rooms.%d/%s_room", rand_num, room[i].name);
            fp = fopen(pathName, "a");

            fprintf(fp, "ROOM NAME: %s\n", room[i].name);
            // Write room connection information to file
            for(j = 0; j < room[i].numOutBoundConnections; j++){
                  fprintf(fp, "CONNECTION %d: %s\n", j + 1, room[i].outBoundConnections[j]->name);
            }
            fprintf(fp, "ROOM TYPE: %s\n", room[i].type);
      }

      // Close file
      fclose(fp);
      return;
}


/*
free room
*/
void free_memory(struct Room* room){
      // Free the memory
      free(room);

      return;
}


// Print all information of rooms, in order to check 
void print_room_information(struct Room* room){
      int i;
      int j;
      for(i = 0; i < 7; i++){
            printf("ROOM NAME: %s\n", room[i].name);
            for(j = 0; j < room[i].numOutBoundConnections; j++){
                  printf("CONNECTON %d: %s\n", j + 1, room[i].outBoundConnections[j]->name);
            }
            printf("ROOM TYPE: %s\n", room[i].type);
            printf("\n");
      }

      return;
}


int main(){
      // Use current time as seed for random generator
      srand(time(0));

      char* room_name[7];
      char* room_type[7];
      int rand_num;

      struct Room* room = create_room();
      rand_num = create_directory();

      get_room_name(room_name);
      get_room_type(room_type);
      set_room_information(room, room_name, room_type);
      create_all_connections(room);
//    print_room_information(room);
      create_file_in_directory(room, rand_num);
      free_memory(room);

      return 0;
}
