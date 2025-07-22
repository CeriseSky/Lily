#include <stdio.h>
#include <stdlib.h>
#include <vector.h>
#include <string.h>
#include <uci.h>
#include <time.h>

#ifndef COMMIT_HASH
#define COMMIT_HASH "0"
#endif

#define LOG_INFO "[INFO] - "
#define LOG_WARN "[WARN] - "
#define LOG_ERROR "[ERROR] - "

int main() {
  fprintf(stderr, LOG_INFO "Lily version " COMMIT_HASH " by CeriseSky\n"
                  LOG_INFO "Engine debug information is printed to stderr\n");

  UCI_init();
  while(UCI_state.running) {
    char *command = nullptr;
    size_t commandLen = 0;

    ssize_t charsRead = getline(&command, &commandLen, stdin);
    if(charsRead < 0) {
      fprintf(stderr, LOG_ERROR "Failed to read from stdin\n");
      free(command);
      return EXIT_FAILURE;
    }

    clock_t start = clock(), elapsed;

    vec_charptr tokens;
    vec_charptr_new(&tokens);

    char *saveptr;
    for(char *token = strtok_r(command, " \n\r", &saveptr);
        token; token = strtok_r(nullptr, " \n\r", &saveptr))
      if(vec_charptr_push(&tokens, &token) < 0) {
        fprintf(stderr, LOG_ERROR "Failed to parse command\n");
        free(command);
        vec_charptr_delete(&tokens);
        return EXIT_FAILURE;
      }

    elapsed = clock() - start;
    double time = (double)elapsed / (double)CLOCKS_PER_SEC;

    bool handled = false;
    if(tokens.len != 0) {
      for(size_t i = 0; i < UCI_numCommands; i++)
        if(!strcmp(tokens.storage[0], UCI_commands[i].token)) {
          UCI_commands[i].handler(&tokens);
          handled = true;
        }

      if(!handled)
        fprintf(stderr, LOG_WARN "Unhandled command: %s\n", tokens.storage[0]);
    }

    vec_charptr_delete(&tokens);
    free(command);

    elapsed = clock() - start;
    time = (double)elapsed / (double)CLOCKS_PER_SEC;
    fprintf(stderr, LOG_INFO "Command handled in %.2fms\n", time * 1'000.0);
  }

  return EXIT_SUCCESS;
}

