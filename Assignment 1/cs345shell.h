/*
 * Andreas Karampasis csd3777
 * hy345
 * askisi 1 - C Shell
 */
#ifndef HW1_CS345SHELL_H_
#define HW1_CS345SHELL_H_

#include <fcntl.h>
#include <pwd.h>
#include <readline/readline.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0
#define PATH_MAX 1024
#define MAX_ARGUMENTS_SIZE 1024
#define set_green() printf("\033[1;32m")
#define set_blue() printf("\033[1;34m")
#define reset_colour() printf("\033[0m")
#define command_stdout_fd cmd->fd[STDOUT_FILENO]

#define equals(a, b) (strcmp(a, b) == 0)
#define is_single_command(cmds) (cmds->total_commands == 1)
#define get_number_of_pipes() cmds->total_commands - 1
#define get_total_commands() cmds->total_commands
#define stdin_of_command(i) cmds->commands[i]->fds[STDIN_FILENO]
#define stdout_of_command(i) cmds->commands[i]->fds[STDOUT_FILENO]

#define close_all_pipes(pipes, n) \
  for (int i = 0; i < n; ++i) {   \
    close(pipes[i]);              \
  }
#define wait_for_children()                        \
  for (int i = 0; i < get_total_commands(); ++i) { \
    wait(NULL);                                    \
  }
#define execute_commands(c, fds, size)             \
  for (int i = 0; i < get_total_commands(); ++i) { \
    csh_run_cmd(c->commands[i], fds, size);        \
  }

typedef enum redir_type {
  INPUT_REDIR,
  OUTPUT_REDIR,
  APPEND_REDIR,
  INVALID_INPUT
} redir_type;

typedef struct command {
  int argc;                       /* plithos arguments */
  char *name;                     /* onoma entolis */
  char *argv[MAX_ARGUMENTS_SIZE]; /* ta arguments tis entolis */
  int fds[2];                     /* file descriptors gia I/O */
} command_t;

/*TODO: kalytera me list anti gia array */
typedef struct commands {
  int total_commands;      /* plithos entolwn sto pipeline */
  command_t *commands[64]; /* oi entoles sto pipeline */
  redir_type rt;           /* used otan exoume I/O redir */
} commands_t;

/*
 * epistrefei to type gia redirect tis entolis poy dwsame
 */
redir_type prepare_redir_type(char *input);

void csh_print_promt();

/*
 * kanei parse to input to user
 * dimioyrgei to command kai to epistrefei
 */
command_t *csh_parse_input(char *input);

/*
 * kanei parse to input toy user koitontas gia I/O redir
 * arxeikopoioyme to struct commands_t kai to epistrefoyme
 */
commands_t *csh_parse_redir(char *input);

/*
 * kanei parse to input toy user koitontas gia pipes
 * arxeikopoioyme to struct commands_t kai to epistrefoyme
 */
commands_t *csh_parse_pipes(char *input);

/*
 * pairnei tis entoles san input
 * an einai mia entoli tote tin ekteloume kai telos
 * an exoume pipelined entoles tote ftiaxnoumem ta pipes
 * gia tis entoles, allazoume ta file descriptors twn commands
 * gia na tis trexoume kai telos kleinoyme ta pipes kai perimenoyme
 * na teleiwsoun ola ta paidia
 */
int chs_run_pipes(commands_t *cmds);

/*
 * kanoyme set ta file descriptors analoga me to reditection
 * kai ekteloume tin entoli
 */
int csh_run_redir(commands_t *cmds);

/*
 * ektelei tin entoli poy toy leei i function csh_run_pipes
 * kaloyme to fork gia dimiourgia neas process
 * i nea diergasia pairnei ta file desccriptors poy tis exoume
 * valei otan kaname to parse kai elegxei gia tyxon lathi
 */
int csh_run_cmd(command_t *cmd, int pipefd[], int pipes_count);

/* enas aplos signal handler */
void sig_handler(int signo);
#endif
