#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


struct Room{
      char name[16];
      char type[16];
      int numOutBoundConnections;
      char outBoundConnections[6][32];
};


struct Step{
      int num_step;
      char steps[100][10];
};

/*
create a struct room
*/
struct Room* create_room(){
      // Create a room struct
      struct Room* room = malloc(7 * sizeof(struct Room));

      int i;
      int j;
      for(i = 0; i < 7; i++){
            // initialize room array information
            memset(room[i].name, '\0', sizeof(room[i].name));
            memset(room[i].type, '\0', sizeof(room[i].type));
            room[i].numOutBoundConnections = 0;
            for(j = 0; j < 6; j++){
                  // initialize connection room array information
                  memset(room[i].outBoundConnections[j], '\0', sizeof(room[i].outBoundConnections[j]));
            }
      }
      return room;
}


/*
Create step sturct
*/
struct Step create_step(){
      int i = 0;
      // Create a step struct
      struct Step steps;

      steps.num_step = 0;
      // initialize step array information
      for(i = 0; i < 100; i++){
            memset(steps.steps[i], '\0', sizeof(steps.steps[i]));
      }
      return steps;
}


/*
open up the current directory ang get a directory's name that begin with the pathname:"jiaha.rooms.".
The Code snippet from Canvas
*/
void read_directory(char newestDirName[256]){
      // Modified timestamp of newest subdir examined and Prefix we are looking for
      int newestDirTime = -1;
      char targetDirPrefix[32] = "jiaha.rooms.";
      memset(newestDirName, '\0', sizeof(newestDirName));

      // Holds the directory we are starting in
      // Holds the current subdir of the starting dir
      // Holds information we have gained about subdir
      DIR* dirToCheck;
      struct dirent *fileInDir;
      struct stat dirAttributes;

      // Open up the directory this program was run in
      dirToCheck = opendir(".");

      // Make sure the current directory could be opened
      if(dirToCheck > 0){
            // Check each entry in dir
            while((fileInDir = readdir(dirToCheck)) != NULL){
                  if(strstr(fileInDir->d_name, targetDirPrefix) != NULL){
                        // Get attributes of the entry
                        stat(fileInDir->d_name, &dirAttributes);

                        // If this time is bigger
                        if((int)dirAttributes.st_mtime > newestDirTime){
                              newestDirTime = (int)dirAttributes.st_mtime;
                              memset(newestDirName, '\0', sizeof(newestDirName));
                              strcpy(newestDirName, fileInDir->d_name);
                        }
                  }
            }
      }
      // Close the directory we opened
      closedir(dirToCheck);
}


/*
Open the directory of jiaiha.rooms, and read the room file
The aim of this function is to get the room name and store to room_array
The Code snippet from Canvas
*/
void get_file_names(char newestDirName[256], char room_name[7][20]){
      int i = 0;
      int size = 0;

      // Modified timestamp of newest subdir examined and Prefix we are looking for
      int newestDirTime = -1;
      char targetDirPrefix[32] = "_room";

      // Holds the directory we are starting in
      DIR* dirToCheck;
      struct dirent *fileInDir;
      struct stat dirAttributes;

      // Open up the directory this program was run in
      dirToCheck = opendir(newestDirName);

      // Make sure the current directory could be opened
      if (dirToCheck > 0){
            // Check each entry in dir
            while ((fileInDir = readdir(dirToCheck)) != NULL){
                  if (strstr(fileInDir->d_name, targetDirPrefix) != NULL){
                        // get the room name and store to room_array
                        memset(room_name[i], '\0', sizeof(room_name[i]));
                        size = strlen(fileInDir->d_name);
                        strncpy(room_name[i], fileInDir->d_name, size - 5);
                        i++;
                  }
           }
     }
     // If open directory faile, print error message
     else{
          printf("The directory %s cannot be opened.\n", newestDirName);
          exit(1);
    }

    closedir(dirToCheck);
    return;
}


/*
create directory and get each room information
*/
void get_room_information(struct Room* room, char room_name[7][20], char newestDirName[256]){
      int i = 0;
      int size = 0;
      char pathFile[50];
      char fgets_l [50];
      char tempGets[50];

      // The initialization file
      FILE *fp;
      for(i = 0; i < 7; i++){
            // open a file
            sprintf(pathFile, "./%s/%s_room", newestDirName, room_name[i]);
            fp = fopen(pathFile, "r");

            // Check whether the file open
            if(fp == NULL){
                  printf("The file %s cannot be opened.\n", pathFile);
                  exit(1);
            }

            // while loop to read every room file
            int j = 0;
            while(fgets(fgets_l, 50, fp)){
                  // copy the "getline"
                  strcpy(tempGets, fgets_l);
                  // get the size of the line
                  size = strlen(tempGets);
                  // split each line by " "
                  char* split = strtok(fgets_l, " ");

                  // The first line must be room name
                  if(j == 0){
                        strncpy(room[i].name, tempGets + 11, size - 12);
                  }
                  // if the line include "CONNECTION", add the room into connection
                  else if(strcmp(split, "CONNECTION") == 0){
                        strncpy(room[i].outBoundConnections[j-1], tempGets+14, size-15);
                        // the number of connection room plus one
                        room[i].numOutBoundConnections ++;
                  }
                  // get the type of the room
                  else{
                        strncpy(room[i].type, tempGets+11, size-12);
                  }
                  j++;
            }
      }

      // Close the file
      fclose(fp);
      return;
}


/*
Get start room index
*/
int get_start_room(struct Room* room){
      int i;

      for(i = 0; i < 7; i++){
            if(strcmp(room[i].type, "START_ROOM") == 0){
                  return i;
            }
      }
}


/*
print connecton rooms' information
*/
void print_connectons_room(struct Room* room, int start_index){
      int i;
      for(i = 0; i < room[start_index].numOutBoundConnections; i++){
            // The last room following ".", others following ","
            if(i == room[start_index].numOutBoundConnections - 1){
                  printf("%s.\n", room[start_index].outBoundConnections[i]);
            }
            else{
                  printf("%s, ", room[start_index].outBoundConnections[i]);
            }
      }
}


/*
Check the input if is a valid room name
*/
int check_input(struct Room* room, char input[100], int start_index){
      int i;
      for(i = 0; i < room[start_index].numOutBoundConnections; i++){
            // if input is a valid name, return 1
            if(strcmp(input, room[start_index].outBoundConnections[i]) == 0){
                  return 1;
            }
      }
      // If input is an error name, return 2
      return 2;

}


/*
When receive a room name, get index of the room
*/
int get_room_index(struct Room* room, char input[]){
      int i;
      for(i = 0; i < 7; i++){
            // if the room name equal input, return the index
            if(strcmp(input, room[i].name) == 0){
                  return i;
            }
      }
}


/*
Check if the input is the end room, if it is "end" room, return 1
*/
int check_end_room(struct Room* room, char input[]){
      int index = 0;
      // call get_room_index()
      index = get_room_index(room, input);

      // check if the room is "END_ROOM"
      if(strcmp(room[index].type, "END_ROOM") == 0){
            return 1;
      }
      return 0;
}


/*
print congratulation information
*/
void print_congratulation_info(struct Step steps){
      int i;

      printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
      printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps.num_step);

      // print adventure steps information
      for(i = 0; i < steps.num_step; i++){
            printf("%s\n", steps.steps[i]);
      }
      return;
}


/*
adventure loops, display connecton informations or error informations
*/
void loop(struct Room* room, struct Step steps, char input[], int index){
      int flag = 1;
      int result = 0;
      int if_win = 0;
      int index_2 = 0;

      while(flag == 1){
            // check the input of the user whether is correct
            result = check_input(room, input, index);

            switch(result){
                  // if the user enter a right room name
                  // list the possible connections that can be followed
                  case 1:
                        // add information to step
                        strcpy(steps.steps[steps.num_step], input);
                        steps.num_step += 1;

                        // check if the user get the "END_ROOM"
                        if_win = check_end_room(room, input);
                        if(if_win == 1){
                              // print congratulation informations
                              print_congratulation_info(steps);
                              exit(0);
                        }

                        // get the new room index
                        index_2 = get_room_index(room, input);

                        // display current location and prompt
                        printf("CURRENT LOCATION: %s\n", room[index_2].name);
                        printf("POSSIBLE CONNECTIONS: ");

                        // print connecton room names
                        print_connectons_room(room, index_2);
                        printf("WHERE TO? >");
                        // ask user to input again
                        scanf("%s", input);
                        printf("\n");

                        index = index_2;
                        break;

                  // If the user types anything but a valid room name from this location,
                  // the game should return an error line
                  case 2:
                        printf("HUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
                        printf("\n");

                        // repeat the current location and prompt
                        printf("CURRENT LOCATION: %s\n", room[index].name);
                        printf("POSSIBLE CONNECTIONS: ");
                        // print connecton room names
                        print_connectons_room(room, index);
                        printf("WHERE TO? >");
                        // ask user to input again
                        scanf("%s", input);
                        printf("\n");
                        break;
            }
      }
}


/*
adventure function
*/
void adventure(struct Room* room){
      struct Step steps = create_step();
      char input[100];
      int index = get_start_room(room);

      // display the current location and prompt
      printf("CURRENT LOCATION: %s\n", room[index].name);
      printf("POSSIBLE CONNECTIONS: ");
      // print connecton room names
      print_connectons_room(room, index);
      printf("WHERE TO? >");
      // ask user to input again
      scanf("%s", input);
      printf("\n");

      loop(room, steps, input, index);
}


/*
Free memory
*/
void free_memory(struct Room* room){
      free(room);
      return;
}


/*
prinit room name, check my code whether correct
*/
void print_room_name(char room_name[7][20]){
      int i;
      int j;
      for(i = 0; i < 7; i++){
            printf("Room %d: %s\n", i+1, room_name[i]);
      }
}


/*
prinit room information, check my code whether correct
*/
void print_room_information(struct Room* room){
      int i;
      int j;
      for(i = 0; i < 7; i++){
            printf("ROOM NAME: %s\n", room[i].name);
            for(j = 0; j < room[i].numOutBoundConnections; j++){
                  printf("CONNECTION %d: %s\n", j + 1, room[i].outBoundConnections[j]);
            }
            printf("ROOM TYPE: %s\n", room[i].type);
            printf("\n");
      }

      return;
}


int main(){
      struct Room* room = create_room();
      char newestDirName[256];
      char room_name[7][20];

      read_directory(newestDirName);
      get_file_names(newestDirName, room_name);
//      print_room_name(room_name);
      get_room_information(room, room_name, newestDirName);
//      print_room_information(room);
      adventure(room);
      free_memory(room);
}
