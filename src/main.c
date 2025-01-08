#include "main.h"

int main(int argc, char *argv[]) {
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  if (argc < 2) {
    fprintf(stderr, "Usage: %s <command> [<args>]\n", argv[0]);
    return 1;
  }

  const char *command = argv[1];

  if (strcmp(command, "init") == 0) {
    return command_init(argc, argv);
  } else if (strcmp(command, "cat-file") == 0) {
    return command_cat_file(argc, argv);
  } else if (strcmp(command, "hash-object") == 0) {
    return command_hash_object(argc, argv);
  } else {
    fprintf(stderr, "Unknown command %s\n", command);
    return 1;
  }

  return 0;
}
