/*
 * Andreas Karampasis csd3777
 * hy345
 * askisi 1 - C Shell
 */

#include "cs345shell.h"

int main() {
  commands_t *cmds;
  char *user_input;
  /* kanoyme overide ton default signal handler
   * wste na mi skotwnoyme to shell mas me to CTRL+C */
  signal(SIGINT, SIG_IGN);
  while (TRUE) {
    csh_print_promt();
    user_input = readline(NULL);
    if (!user_input) {
      exit(EXIT_SUCCESS);
    }
    // :(
    if (strstr(user_input, "|")) {
      cmds = csh_parse_redir(user_input);
      if (cmds->total_commands == 0) {
        free(user_input);
        continue;
      }
      csh_run_redir(cmds);
    } else {
      cmds = csh_parse_pipes(user_input);
      if (cmds->total_commands == 0) {
        free(user_input);
        continue;
      }
      chs_run_pipes(cmds);
    }
    free(cmds);
    free(user_input);
  }
  return 0;
}