#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define TERMINAL_RL_BUFSIZE 1024
#define MAX_ARGS 64
#define PATH_MAX 1024

char *show_dir() {
  char *cwd = malloc(PATH_MAX);
  if (getcwd(cwd, PATH_MAX) == NULL) {
    perror("Current path error\n");
    free(cwd);
    exit(EXIT_FAILURE);
  }
  return cwd;
}

void terminal_read_line(char *input) {
  int bufSize = TERMINAL_RL_BUFSIZE;
  char *dir = show_dir();
  char target[] = "exit";

  printf("%s \n$ ", dir);
  if (fgets(input, bufSize, stdin) == NULL) {
    free(dir);
    exit(EXIT_FAILURE);
  }
  input[strcspn(input, "\n")] = 0;

  if (strcmp(target, input) == 0) {
    exit(EXIT_FAILURE);
  }
  free(dir);
}

void terminal_split_line(char *input, char **args) {
  int i = 0;

  char delimiter[] = " "; 
  args[i] = strtok(input, delimiter);

  while (args[i] != NULL && i < MAX_ARGS) {
    printf("Token: %s\n", args[i]);

    i++;
    args[i] = strtok(NULL, delimiter);
  }
}

int handle_build_in(char **args) {
  char *command = args[0];
  char *path = args[1];

  if (command == NULL) return 1;
  
  if (strcmp("clear", command) == 0) {
    system("clear");
  } else if (strcmp("cd", command) == 0) {
    if (path != NULL) {
      if (chdir(path) != 0) {
        perror(path);
      } 
    }
  } 
  return 0;
}

void execute_command(char **args) {
  pid_t pid = fork();

  if (pid < 0) {
    perror("fork failed");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    if (execvp(args[0], args) != 0) {
      perror("Executing command error");
    }
  } else if (pid > 0) {
    wait(NULL);
  }
  printf("process_pid = %d\n", getpid());
}

int main() {
  char input[TERMINAL_RL_BUFSIZE];
  char *args[MAX_ARGS];

  while (1) {
    terminal_read_line(input);
    terminal_split_line(input, args);

    if (handle_build_in(args)) continue;
    execute_command(args);
  }
  return 0;
}
