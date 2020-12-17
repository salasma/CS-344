#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
Generate key file
*/
void print_key(int keylength){
      int i;
      int random;
      char key[keylength+1];
      char operation[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

      for(i = 0; i < keylength; i++){
            // random generate a number between 0 to 26
            random = rand() % 27;
            // the index of arry is ' '
            if(random == 26){
                  key[i] = ' ';
            }
            else{
                  key[i] = operation[random];
            }
      }
      key[keylength] = '\0';
      printf("%s\n", key);
      return;
}


int main(int argc, char* argv[]){
      // Use current time as
      // seed for random generator
      srand(time(0));

      // Check command line arguments
      if(argc < 2){
            fprintf(stderr, "Need more arguments\n");
            exit(0);
      }

      print_key(atoi(argv[1]));
      return 0;
}
