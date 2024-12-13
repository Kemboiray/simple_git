#include "main.h"

int command_cat_file(int argc, char *argv[]) {
  const char *usage = "Usage: %s cat-file -p <hash>\n";
  if (argc < 4) {
    fprintf(stderr, usage, argv[0]);
    return 1;
  }
  const char *flag = argv[2];
  const char *hash = argv[3];
  static char path_buf[55];
  memset(path_buf, 0, 55);
  if (strcmp(flag, "-p") == 0) {
    if (strlen(hash) != 40) {
      fprintf(stderr, "fatal: Not a valid object name %s\n", hash);
      return 1;
    }
    const char dir[] = {hash[0], hash[1], '/', 0};
    strcpy(path_buf, ".git/objects/");
    strcpy(path_buf + 13, dir);
    strcpy(path_buf + 16, &(hash[2]));
    FILE *object_file = fopen(path_buf, "r");
    if (object_file == NULL) {
      fprintf(stderr, "fatal: Not a valid object name %s\n%s: %s\n", hash,
              strerror(errno), path_buf);
      return 1;
    }
  } else {
    fprintf(stderr, usage, argv[0]);
    return 1;
  }
  return 0;
}
