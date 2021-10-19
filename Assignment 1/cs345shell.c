/*
 * Andreas Karampasis csd3777
 * hy345
 * askisi 1 - C Shell
 */

#include "cs345shell.h"

/*
 * epistrefei to type gia redirect tis entolis poy dwsame
 */
redir_type prepare_redir_type(char *input) {
  if (strstr(input, "|||")) {
    return APPEND_REDIR;
  } else if (strstr(input, "||")) {
    return OUTPUT_REDIR;
  } else if (strstr(input, "|")) {
    return INPUT_REDIR;
  }
  // an ftasoume pote edw tote exoume dwsei lathos input gia redirect
  return INVALID_INPUT;
}

void csh_print_promt() {
  char path[PATH_MAX];
  char *user;
  // h getlogin gia kapoio logo moy ebgaze NULL ton user sto vm
  struct passwd *pwd = getpwuid(getuid());
  if (pwd) {
    user = pwd->pw_name;
  }
  getcwd(path, sizeof(path));
  set_green();
  printf("[cs345sh][%s]", user);
  set_blue();
  printf("~[%s]", path);
  reset_colour();
  printf("# ");
}

/*
 * kanei parse to input to user
 * dimioyrgei to command kai to epistrefei
 */
command_t *csh_parse_input(char *input) {
  char *token;
  int argc = 0;
  command_t *command;
  // pio grigoro apo malloc + memset
  command = calloc(sizeof(command_t) + MAX_ARGUMENTS_SIZE * sizeof(char *), 1);
  if (command == NULL) {
    fprintf(stderr, "ERROR::PARSE::COMMAND::CALLOC\n");
    exit(EXIT_FAILURE);
  }
  token = strtok(input, " ");
  while (token != NULL) {
    command->argv[argc] = token;
    ++argc;
    token = strtok(NULL, " ");
  }
  command->name = command->argv[0];
  command->argc = argc;
  return command;
}

/*
 * kanei parse to input toy user koitontas gia I/O redir
 * arxeikopoioyme to struct commands_t kai to epistrefoyme
 * :(
 */
commands_t *csh_parse_redir(char *input) {
  commands_t *cmds;
  redir_type rt;  // flag gia na kserw ti redirect na kanw
  int i = 0;
  char *token, *ptr, *delim;

  cmds = calloc(sizeof(command_t) + MAX_ARGUMENTS_SIZE * sizeof(char *), 1);

  if (!cmds) {
    fprintf(stderr, "ERROR::PARSE::PIPE::CALLOC\n");
    exit(EXIT_FAILURE);
  }
  rt = prepare_redir_type(input);
  // >:(
  switch (rt) {
    case INPUT_REDIR:
      delim = strdup("|");
      cmds->rt = INPUT_REDIR;
      break;
    case OUTPUT_REDIR:
      delim = strdup("||");
      cmds->rt = OUTPUT_REDIR;
      break;
    case APPEND_REDIR:
      delim = strdup("|||");
      cmds->rt = APPEND_REDIR;
      break;
    default:
      exit(EXIT_FAILURE);
      break;
  }
  /*
   * xreiazomaste strtok_r giati tin kaloyme gia to idio string
   * me tin strtok den doyleve opws tha thelame
   */
  token = strtok_r(input, delim, &ptr);
  while (token != NULL) {
    cmds->commands[i] = csh_parse_input(token);
    token = strtok_r(NULL, delim, &ptr);
    ++i;
  }
  cmds->total_commands = i;

  return cmds;
}

/*
 * kanoyme set ta file descriptors analoga me to reditection
 * kai ekteloume tin entoli
 */
int csh_run_redir(commands_t *cmds) {
  char *file_name = cmds->commands[1]->name;
  switch (cmds->rt) {
    case INPUT_REDIR:
      // stdin erxetai apo to input file
      stdin_of_command(0) = open(file_name, O_RDONLY);
      // Den allazei to stdout
      stdout_of_command(0) = STDOUT_FILENO;
      break;
    case OUTPUT_REDIR:
      // Den allazei to stdin
      stdin_of_command(0) = STDIN_FILENO;
      // stdout pigainei sto sto output file
      stdout_of_command(0) = creat(file_name, 0644);
      break;
    case APPEND_REDIR:
      // Den allazei to stdin
      stdin_of_command(0) = STDIN_FILENO;
      // stdout pigainei sto sto output file
      stdout_of_command(0) =
          open(file_name, O_WRONLY | O_APPEND | O_CREAT, 0644);
      break;
    default:
      exit(EXIT_FAILURE);
      break;
  }
  csh_run_cmd(cmds->commands[0], NULL, 0);
  // wait_for_children();
  wait(NULL);

  return 0;
}

/*
 * kanei parse to input toy user koitontas gia pipes
 * arxeikopoioyme to struct commands_t kai to epistrefoyme
 */
commands_t *csh_parse_pipes(char *input) {
  commands_t *cmds;
  int i = 0;
  char *token, *ptr;  // used gia to strtok_r

  cmds = calloc(sizeof(commands_t) + i * sizeof(command_t), 1);
  if (!cmds) {
    fprintf(stderr, "ERROR::PARSE::PIPE::CALLOC\n");
    exit(EXIT_FAILURE);
  }
  /*
   * xreiazomaste strtok_r giati tin kaloyme gia to idio string
   * me tin strtok den doyleve opws tha thelame
   */
  token = strtok_r(input, ">", &ptr);
  while (token != NULL) {
    cmds->commands[i] = csh_parse_input(token);
    token = strtok_r(NULL, ">", &ptr);
    ++i;
  }
  cmds->total_commands = i;
  return cmds;
}

/*
 * pairnei tis entoles san input
 * an einai mia entoli tote tin ekteloume kai telos
 * an exoume pipelined entoles tote ftiaxnoumem ta pipes
 * gia tis entoles, allazoume ta file descriptors twn commands
 * gia na tis trexoume kai telos kleinoyme ta pipes kai perimenoyme
 * na teleiwsoun ola ta paidia
 */
int chs_run_pipes(commands_t *cmds) {
  int total_pipes = get_number_of_pipes();
  const int size_of_pipeline = total_pipes * 2;
  int i, pipefd[size_of_pipeline];
  /* an einai mia entoli tote tin ekteloyme xwris pipeline */
  if (is_single_command(cmds)) {
    csh_run_cmd(cmds->commands[0], NULL, 0);
    wait(NULL);
  } else {
    for (i = 0; i < total_pipes; ++i) {
      if (pipe(pipefd + (2 * i)) < 0) {
        fprintf(stderr, "ERROR::PIPE::INIT\n");
        return 0;
      }
    }
    stdin_of_command(i) = STDIN_FILENO;
    for (i = 0; i < total_pipes; ++i) {
      stdout_of_command(i) = pipefd[(i * 2) + 1];
      stdin_of_command(i + 1) = pipefd[i * 2];
    }
    stdout_of_command(i) = STDOUT_FILENO;

    execute_commands(cmds, pipefd, size_of_pipeline);
    close_all_pipes(pipefd, size_of_pipeline);
    wait_for_children();
  }
  return 1;
}

/*
 * ektelei tin entoli poy toy leei i function csh_run_pipes
 * kaloyme to fork gia dimiourgia neas process
 * i nea diergasia pairnei ta file desccriptors poy tis exoume
 * valei otan kaname to parse kai elegxei gia tyxon lathi
 */
int csh_run_cmd(command_t *cmd, int pipefd[], int pipes_count) {
  pid_t pid;
  int fd[2];
  if (equals(cmd->name, "exit")) {
    exit(EXIT_SUCCESS);
  } else if (equals(cmd->name, "cd")) {
    if (chdir(cmd->argv[1]) != 0) {
      fprintf(stderr, "cd: %s: No such file or directory\n", cmd->argv[1]);
      return 1;
    }
    return 0;
  }
  pid = fork();
  if (pid == -1) {
    fprintf(stderr, "ERROR::FORK\n");
    return 0;
  } else if (pid == 0) {
    signal(SIGINT, sig_handler);

    fd[STDIN_FILENO] = cmd->fds[STDIN_FILENO];
    fd[STDOUT_FILENO] = cmd->fds[STDOUT_FILENO];

    if (dup2(fd[STDIN_FILENO], STDIN_FILENO) < 0) {
      fprintf(stderr, "ERROR::EXEC::DUP::FD::STDIN_FILENO\n");
      exit(EXIT_FAILURE);
    }
    if (dup2(fd[STDOUT_FILENO], STDOUT_FILENO) < 0) {
      fprintf(stderr, "ERROR::EXEC::DUP::FD::STDOUT_FILENO\n");
      exit(EXIT_FAILURE);
    }
    // an exoume pipes frontizoume na ta kleisoume
    if (pipefd) {
      close_all_pipes(pipefd, pipes_count);
    }
    execvp(cmd->name, cmd->argv);
    // an ftasoume edw tote rip
    fprintf(stderr, "%s: command not found\n", cmd->name);
    exit(EXIT_FAILURE);
  }
  return pid;
}

/* enas aplos signal handler */
void sig_handler(int signo) {
  switch (signo) {
    case SIGINT:
      signal(SIGINT, SIG_DFL);
      break;

    default:
      break;
  }
}
