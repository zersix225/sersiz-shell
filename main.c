#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

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

  printf("%s \n$ ", dir);
  if (fgets(input, bufSize, stdin) == NULL) {
    free(dir);
    exit(EXIT_FAILURE);
  }
  input[strcspn(input, "\n")] = 0;

  free(dir);
}

void terminal_split_line(char *input, char **args, char **left, char **right) {
  int i = 0;
  int j = 0;
  int k = 0;

  char *outer_delimiter = "|";
  char *inner_delimiter = " ";

  char *outer_saveptr = NULL;
  char *inner_saveptr = NULL;

  args[i] = strtok_r(input, outer_delimiter, &outer_saveptr);

  while (args[i] != NULL && i < (MAX_ARGS - 1)) {
    printf("Token: %s\n", args[i]);

    char *inner_token = strtok_r(args[i], inner_delimiter, &inner_saveptr);
 
    while (inner_token != NULL) {
      printf("Inner Token: %s\n", inner_token);

      if (i == 0)  {
        left[j] = inner_token;
        j++;

      } else if (i == 1) {
        right[k] = inner_token;
        k++;
      }
      inner_token = strtok_r(NULL, inner_delimiter, &inner_saveptr);
    }

    i++;
    args[i] = strtok_r(NULL, outer_delimiter, &outer_saveptr);
  }
  left[j] = NULL;
  right[k] = NULL;
  args[i] = NULL;
}

int handle_build_in(char **args) {
  char *command = args[0];
  char *path = args[1];

  if (command == NULL) return 1;
  
  if (strcmp("clear", command) == 0) {
    system("clear");
  } else if (strcmp(command, "exit") == 0) {
    exit(EXIT_SUCCESS);
  } else if (strcmp("cd", command) == 0) {
    if (path != NULL) {
      if (chdir(path) != 0) {
        perror(path);
      } 
    }
  } 
  return 0;
}

void execute_command(int *fd1, char **left, char **right) {
  pid_t pid;

  if (pipe(fd1) < 0) {
    perror("pipe1 failed");
  }

  pid = fork();
  if (pid < 0) {
    perror("fork1 failed");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    if (dup2(fd1[1], STDOUT_FILENO) < 0) perror("dup2 c1");
    close(fd1[0]); close(fd1[1]);
    execvp(left[0], left);

    perror("execvp left");
    exit(1);
  }

  pid = fork();
  if (pid < 0) {
    perror("fork2 failed");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    if (dup2(fd1[0], STDIN_FILENO) < 0) perror("dup2 c2 in");
    close(fd1[0]); close(fd1[1]);
    execvp(right[0], right);

    perror("execvp right");
    exit(1);
  }

  close(fd1[0]); close(fd1[1]);
  wait(NULL);
  wait(NULL);
  printf("process_pid = %d\n", getpid());
}

int main(int argc, char *argv[]) {
  char input[TERMINAL_RL_BUFSIZE];
  char *args[MAX_ARGS];
  char *left[MAX_ARGS];
  char *right[MAX_ARGS];

  int fd1[2];

  while (1) {
    terminal_read_line(input);
    terminal_split_line(input, args, left, right);

    if (handle_build_in(args)) continue;
    execute_command(fd1, left, right);
  }
  return 0;
}
