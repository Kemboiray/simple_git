#include "main.h"

int is_valid_flag(const char *flag) { return strcmp(flag, "-w") == 0; }

int command_hash_object(int argc, char *argv[]) {
  const char *usage = "Usage: %s hash-object [-w] <file>\n";

  if (argc != 3 && argc != 4) {
    fprintf(stderr, usage, argv[0]);
    return 1;
  }

  if (argc == 4 && !is_valid_flag(argv[2])) {
    fprintf(stderr, usage, argv[0]);
    return 1;
  }

  const char *flag = argc == 4 ? argv[2] : NULL;
  const char *file_name = flag == NULL ? argv[2] : argv[3];

  FILE *input_stream =
      (strcmp(file_name, "-") == 0 || strcmp(file_name, "--stdin") == 0)
          ? stdin
          : fopen(file_name, "rb");
  if (input_stream == NULL) {
    perror("command_hash_object");
    return 1;
  }

  FILE *output_stream = tmpfile();
  if (output_stream == NULL) {
    perror("tmpfile");
    fclose(input_stream);
    return 1;
  }

  long bytes = copy_bytes(output_stream, input_stream);
  if (bytes == -1) {
    fclose(input_stream);
    fclose(output_stream);
    return 1;
  }
  fclose(input_stream);

  if (fseek(output_stream, 0, SEEK_SET) != 0) {
    perror("fseek");
    fclose(output_stream);
    return 1;
  }

  FILE *source = tmpfile();
  if (source == NULL) {
    perror("tmpfile");
    fclose(output_stream);
    return 1;
  }

  if (fprintf(source, "blob %li", bytes) < 0) {
    perror("fprintf");
    fclose(output_stream);
    fclose(source);
    return 1;
  }

  if (fputc(0, source) < 0) {
    perror("fputc");
    fclose(output_stream);
    fclose(source);
    return 1;
  }

  char in_buffer[8192] = {0};
  size_t bytes_read;
  while ((bytes_read = fread(in_buffer, 1, sizeof(in_buffer), output_stream)) >
         0) {
    if (fwrite(in_buffer, 1, bytes_read, source) != bytes_read) {
      perror("fwrite");
      fclose(output_stream);
      fclose(source);
      return 1;
    }
  }
  if (fflush(source) != 0) {
    perror("fflush");
    fclose(source);
    return 1;
  }

  fclose(output_stream);
  if (fseek(source, 0, SEEK_SET) != 0) {
    perror("fseek");
    fclose(source);
    return 1;
  }

  size_t hash_bufsize = EVP_MAX_MD_SIZE * 2 + 1;
  char hash_buffer[hash_bufsize];
  char *hash = hash_file(source, hash_buffer, hash_bufsize);
  if (hash == NULL) {
    fclose(source);
    return 1;
  }
  puts(hash);

  if (flag) {
    if (fseek(source, 0, SEEK_SET) != 0) {
      perror("fseek");
      fclose(source);
      return 1;
    }

    char *hash_object_path = mk_git_object_path(hash);
    if (hash_object_path == NULL) {
      fclose(source);
      return 1;
    }
    unsigned n = strlen(hash_object_path) - 38;
    char hash_object_dir[n];
    strncpy(hash_object_dir, hash_object_path, n);
    hash_object_dir[n - 1] = 0;

    if (access(hash_object_dir, F_OK) != 0) {
      if (mkdir(hash_object_dir, 0755) == -1) {
        perror("mkdir");
        free(hash_object_path);
        fclose(source);
        return 1;
      }
    }

    FILE *dest = fopen(hash_object_path, "wb");
    if (dest == NULL) {
      perror("fopen");
      if (rmdir(hash_object_dir) == -1)
        perror("rmdir");
      free(hash_object_path);
      fclose(source);
      return 1;
    }

    int ret = def(source, dest, Z_DEFAULT_COMPRESSION);
    if (ret != Z_OK) {
      zerr(ret, source, dest);
      fclose(dest);
      fclose(source);
      if (remove(hash_object_path) == -1) {
        perror("remove");
      } else {
        if (rmdir(hash_object_dir) == -1)
          perror("rmdir");
      }
      free(hash_object_path);
      return 1;
    }

    fclose(dest);
    free(hash_object_path);
  }
  fclose(source);

  return 0;
}
