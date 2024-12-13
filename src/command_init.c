#include "main.h"

int command_init(void) {
  fprintf(stderr, "Logs from your program will appear here!\n");

  if (mkdir(".git", 0755) == -1 || mkdir(".git/objects", 0755) == -1 ||
      mkdir(".git/refs", 0755) == -1 || mkdir(".git/refs/heads", 0755) == -1) {
    fprintf(stderr, "Failed to create directories: %s\n", strerror(errno));
    return 1;
  }

  FILE *headFile = fopen(".git/HEAD", "w");
  if (headFile == NULL) {
    fprintf(stderr, "Failed to create .git/HEAD file: %s\n", strerror(errno));
    return 1;
  }
  fprintf(headFile, "ref: refs/heads/main\n");
  fclose(headFile);

  printf("Initialized git directory\n");
  return 0;
}
