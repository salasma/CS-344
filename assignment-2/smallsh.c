#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>


int status = 0;
int if_background = 0;
int if_foreground = 0;
int background_PID[100];
int background_num = 0;
int argu_size = 0;
int shell_pid = 0;
char* argu[2048];
char* build_in[] = {"cd", "exit", "status"};
int build_in_size = 3;


/*
check the non-build in commands if include "<" or ">"
*/
void non_build_in_check(){
      int i = 0;

      // check all argument
      for(i = 0; i < argu_size; i++){
            // Check if the argument is "<"
            if(strcmp(argu[i], "<") == 0 && argu[i+1]) {
                  // Open source file
                  int infile = open(argu[i + 1], O_RDONLY);
                  if(infile == -1){
                        status = 1;
                        perror("Error: can not open this file.\n");
                        fflush(stdout);
                  }
                  // Redirect stdin to source file
                  dup2(infile, 0);
                  fcntl(infile, F_SETFD, FD_CLOEXEC);
                  argu[i] = NULL;
            }
            // Check if the argument is ">"
            else if(strcmp(argu[i], ">") == 0 && argu[i+1]){
                  // Open source file
                  int outfile = open(argu[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
                  if(outfile == -1){
                        status = 1;
                        perror("Error: can not open this file.\n");
                        fflush(stdout);
                  }
                  // Redirect stdout to source file
                  dup2(outfile, 1);
                  fcntl(outfile, F_SETFD, FD_CLOEXEC);
                  argu[i] = NULL;
            }
      }
      return;
}


/*
execute non-build in commands
*/
void non_build_in_execute(){
      int infile = 0;
      int outfile = 0;
      int childStatus;

      // Fork a new process
      pid_t spawnPid = fork();

      if(spawnPid == -1){
            perror("fork() failed!\n");
            exit(1);
      }
      else if(spawnPid == 0){
            // Set the foreground crtl-c to default
            if(if_background == 0){
                  // Initialize SIGINT_action struct to be empty
                  struct sigaction SIGINT_action = {0};
                  // Fill out the SIGINT_action struct
                  // Register handle_SIGINT as the SIG_DFL
                  SIGINT_action.sa_handler = SIG_DFL;
                  // Block all catchable signals while SIGINT_action is running
                  sigfillset(&SIGINT_action.sa_mask);
                  // No flags set
                  SIGINT_action.sa_flags = 0;
                  sigaction(SIGINT, &SIGINT_action, NULL);
            }
            else if(if_background == 1){
                  infile = open("/dev/null", O_WRONLY);
                  dup2(infile, STDOUT_FILENO);
                  dup2(infile, STDERR_FILENO);

                  outfile = open("/dev/null", O_RDONLY);
                  dup2(outfile, STDIN_FILENO);
            }
            non_build_in_check();
            // Replace the current program with "/bin/ls"
            execvp(argu[0], argu);
            // exec only returns if there is an error
            perror("execvp");
      }
      else{
            if(if_background == 0){
                  // In the parent process
                  // Wait for child's termination
                  do{
                        // waitpid will immediately return with value 0
                        waitpid(spawnPid, &childStatus, 0);
                  }while(!WIFEXITED(childStatus) && !WIFSIGNALED(childStatus));

                  // This macro returns true if the child was terminated normally.
                  if(WIFEXITED(childStatus)){
                        // If WIFEXITED returned true, WEXITSTATUS will return the status value the child passed to exit()
                        status = WEXITSTATUS(childStatus);
                        if(status != 0){
                              status = 1;
                        }
                  }
                  else{
                        // If WIFSIGNALED returned true, WTERMSIG will return the signal number that caused the child to terminate.
                        printf("terminated by signal %d\n", WTERMSIG(childStatus));
                        fflush(stdout);
                  }
            }
            else if(if_background == 1){
                  // WNOHANG specified. If the child hasn't terminated, waitpid will immediately return with value 0
                  waitpid(spawnPid, &childStatus, WNOHANG);
                  printf("This is a background: %d\n", spawnPid);
                  fflush(stdout);
                  // Store the background id into array, and plus the background number
                  background_PID[background_num] = spawnPid;
                  background_num += 1;
            }
      }
      return;
}


/*
Execute three build in commands
*/
void cd_command(char* path_name){
      // cd to "home" path
      if(chdir(path_name) == 0){
            status = 0;
      }
      else{
            // error messages
            printf("Directory \"%s\" doesn't exist\n", path_name);
            fflush(stdout);
            status = 1;
      }
      return;
}

void exit_command(){
      int i = 0;
      int childStatus = -5;
	int childPid = 0;

      for(i = 0; i < background_num; i++){
            childPid = background_PID[i];
            // WNOHANG specified.
            // If the child hasn't terminated, waitpid will immediately return with value 0
            pid_t spawnPid = waitpid(childPid, &childStatus, WNOHANG);

            if(spawnPid == 0){
                  //fflush(stdout);
                  kill(childPid, SIGKILL);
            }
      }
      exit(0);
      return;
}

void status_command(){
      // Print exit messages
      printf("exit value %d\n", status);
      fflush(stdout);
      return;
}


/*
Execute build in commands
*/
void build_in_execute(int build_in_index){
      // check argument if is build in commands
      if(strcmp(argu[build_in_index], build_in[0]) == 0){
            // check if execute "HOME" or other path
            if(build_in_index == argu_size - 1){
                  cd_command(getenv("HOME"));
            }
            else{
                  cd_command(argu[build_in_index + 1]);
            }
      }
      else if(strcmp(argu[build_in_index], build_in[1]) == 0){
            // Call exit function
            exit_command();
      }
      else if(strcmp(argu[build_in_index], build_in[2]) == 0){
            // Call status function
            status_command();
      }
      return;
}


/*
check input whether build in commands or non-build in commands
*/
void check_input(){
      int i;
      int j;
      if_background = 0;

      for (i = 0; i < argu_size; i++){
            for(j = 0; j < build_in_size; j++){
                  // if this argument is build in commands
                  if(strcmp(argu[i], build_in[j]) == 0){
                        // call build in commands
                        build_in_execute(i);
                        return;
                  }
            }
      }

      // Check if the argument is background
      if(strcmp(argu[argu_size - 1], "&") == 0){
            if_background = 1;
            argu[argu_size - 1] = NULL;
            argu_size -= 1;

            // if (if_foreground) {
            //       if_background = 0;
            // }
      }

      // Call non-build in commands
      non_build_in_execute();
      return;
}

/*
Expand any instance of "$$" in a command into the process ID of the shell itself
*/
void change_PID(){
      int i;
      int j;
      char str_pid[10];

      // Convert int PID to string PID
      sprintf(str_pid, "%d", shell_pid);

      // Check "$$"
      for(i = 0; i < argu_size; i++){
            for(j = 0; j < strlen(argu[i]); j++){
                  if(argu[i][j] == '$' && argu[i][j + 1] == '$'){
                        // Set the "$" to NULL
                        argu[i][j] = '\0';
                        argu[i][j+1] = '\0';
                        // append the shell pid
                        strcat(argu[i], str_pid);
                  }
            }
      }
      return;
}


/*
separate input
*/
void separate_input(char* input){
      input[strlen(input) - 1] = '\0';

      // splitting a string by " "
      // Returns first token
      char* takon = strtok(input, " ");
      int i = 0;
      int j = 0;

      // Keep printing tokens while one of the delimiters present in input.
      while(takon != NULL){
            argu[i] = takon;
            takon = strtok(NULL, " ");
            i++;
      }

      // argument size plus 1
      argu_size = i;

      // Check whether the input is commandline or empty line
      if(argu[0] == NULL || argu[0][0] == '#'){
            return;
      }

      change_PID();
      check_input();
}


/*
check background
Reference: https://repl.it/@cs344/42waitpidexitc#main.c
*/
void check_background(){
      int i = 0;
      int childStatus;
	int childPid;

      for(i = 0; i < background_num; i++){
            childPid = background_PID[i];

            if(childPid == -1){
                  continue;
            }
            // WNOHANG specified.
            // If the child hasn't terminated, waitpid will immediately return with value 0
            pid_t spawnPid = waitpid(childPid, &childStatus, WNOHANG);

            if(spawnPid != 0){
                  background_PID[i] = -1;
                  printf("Background %d terminated.\n", childPid);
                  fflush(stdout);

                  // This macro returns true if the child was terminated normally.
                  if(WIFEXITED(childStatus)){
                        // If WIFEXITED returned true, WEXITSTATUS will return the status value the child passed to exit()
                        printf("Child %d exited normally with status %d\n", childPid, WEXITSTATUS(childStatus));
                        fflush(stdout);
                  }
                  else{
                        // If WIFSIGNALED returned true, WTERMSIG will return the signal number that caused the child to terminate.
                        printf("Child %d exited abnormally due to signal %d\n", childPid, WTERMSIG(childStatus));
                        fflush(stdout);
                  }
            }
      }
      return;
}


/*
catch crtl-z
*/
void catch_crtl_z(int signo){
      // signal handler for SIGINT
      if(if_foreground){
            if_foreground = 0;
            char* display = "Exiting foreground-only mode\n ";
            write(STDOUT_FILENO, display, 32);
      }
      else{
            if_foreground = 1;
            char* display = "Entering foreground-only mode\n(& is ignored)\n ";
            write(STDOUT_FILENO, display, 50);
      }
}


int main(){
      char enter[2048];
      size_t size = 2048;
      memset(enter, '\0', 2048);
      char* input = enter;
      int i = 0;

      // Initialize SIGINT_action struct to be empty
      struct sigaction ignore_action = {0};
      // Fill out the SIGINT_action struct
      // Register handle_SIGINT as the signal handler
      ignore_action.sa_handler = SIG_IGN;
      sigaction(SIGINT, &ignore_action, NULL);

      // Initialize SIGINT_action struct to be empty
	struct sigaction SIGINT_action = {0};
      // Fill out the SIGINT_action struct
      // Register handle_SIGINT as the signal handler
	SIGINT_action.sa_handler = catch_crtl_z;
      // Block all catchable signals while handle_SIGINT is running
	sigfillset(&SIGINT_action.sa_mask);
      // No flags set
	SIGINT_action.sa_flags = SA_RESTART;
      // Install our signal handler
	sigaction(SIGTSTP, &SIGINT_action, NULL);

      // get shell pid
      shell_pid = getpid();

      while(1){
            // reset the argument information
            for(i = 0; i < argu_size; i++){
                  argu[i] = NULL;
            }
            argu_size = 0;
            input = NULL;

            // periodically check for the background child processes to complete
            check_background();
            printf(": ");
            fflush(stdout);
            // get the input
            getline(&input, &size, stdin);
            separate_input(input);
      }
      return 0;
}
