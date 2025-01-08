#include "main.h"

int command_cat_file(int argc, char *argv[]) {
  const char *usage = "Usage: %s cat-file -p <hash>\n";
  if (argc < 4) {
    fprintf(stderr, usage, argv[0]);
    return 1;
  }
  const char *flag = argv[2];
  const char *hash = argv[3];
  char *path = get_git_object_path(hash);
  if (path == NULL) {
    perror("command_cat_file");
    return 1;
  }
  if (strcmp(flag, "-p") == 0) {
    FILE *source = fopen(path, "rb");
    if (source == NULL) {
      perror("command_cat_file:fopen");
      free(path);
      return 1;
    }
    FILE *dest = tmpfile();
    if (dest == NULL) {
      perror("command_cat_file:tmpfile");
      if (fclose(source))
        perror("command_cat_file:fclose");
      free(path);
      return 1;
    }
    int ret = inf(source, dest);
    if (ret != Z_OK) {
      zerr(ret, source, dest);
      int source_status = fclose(source), dest_status = fclose(dest);
      if (source_status || dest_status)
        perror("command_cat_file:fclose");
      free(path);
      return 1;
    }
    if (fclose(source))
      perror("command_cat_file:fclose");
    fseek(dest, 0, SEEK_CUR);
    long dest_size = ftell(dest);
    char *buf = malloc(dest_size);
    if (buf == NULL) {
      perror("command_cat_file:malloc");
      if (fclose(dest))
        perror("command_cat_file:fclose");
      free(path);
      return 1;
    }
    if (fseek(dest, 0, SEEK_SET) != 0) {
      perror("fseek");
      if (fclose(dest))
        perror("command_cat_file:fclose");
      free(path);
      return 1;
    }
    if (fread(buf, 1, dest_size, dest) != dest_size) {
      perror("command_cat_file:fread");
      free(buf);
      if (fclose(dest))
        perror("command_cat_file:fclose");
      free(path);
      return 1;
    }
    char *after_null = memchr(buf, 0, dest_size);
    if (after_null != NULL) {
      after_null++;
      size_t remaining = dest_size - (after_null - buf);
      fwrite(after_null, 1, remaining, stdout);
    }
    free(buf);
    if (fclose(dest))
      perror("command_cat_file:fclose");
  } else {
    fprintf(stderr, usage, argv[0]);
    free(path);
    return 1;
  }
  free(path);
  return 0;
}
