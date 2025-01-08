#include "main.h"

int is_flag(const char *str) { return str[0] == '-' && strlen(str) > 1; }

int is_git_root(const char *path) {
  char git_dir[PATH_MAX];
  snprintf(git_dir, sizeof(git_dir), "%s/.git", path);

  return access(git_dir, F_OK) == 0;
}

char *get_git_root() {
  char current_path[PATH_MAX];
  char resolved_path[PATH_MAX];
  char *resolved_path_dup;

  if (getcwd(current_path, sizeof(current_path)) == NULL) {
    perror("getcwd");
    return NULL;
  }

  while (strlen(current_path)) {
    if (is_git_root(current_path)) {
      if (realpath(current_path, resolved_path) == NULL) {
        perror("realpath");
        return NULL;
      }

      resolved_path_dup = strdup(resolved_path);
      if (resolved_path_dup == NULL) {
        perror("strdup");
        return NULL;
      } else
        return resolved_path_dup;
    }

    char *last_slash = strrchr(current_path, '/');
    if (last_slash == NULL) {
      return NULL;
    }
    *last_slash = '\0';
  }

  return NULL;
}

char *construct_git_object_path(const char *git_root, const char *hash) {
  if (strlen(hash) != 40) {
    perror("create_git_object_path");
    fprintf(stderr, "fatal: Not a valid object name %s\n", hash);
    return NULL;
  }
  char hash_buf[41] = {0};

  for (int i = 0; i < 40; i++) {
    if (!isxdigit(hash[i])) {
      fprintf(stderr, "fatal: Not a valid object name %s\n", hash);
      return NULL;
    }
    hash_buf[i] = tolower(hash[i]);
  }

  char *path = malloc(strlen(git_root) + strlen("/.git/objects/") + 40 + 2);

  if (path == NULL) {
    perror("create_git_object_path");
    return NULL;
  }

  sprintf(path, "%s/.git/objects/", git_root);
  strncat(path, hash_buf, 2);
  strcat(path, "/");
  strcat(path, hash_buf + 2);

  return path;
}

char *get_git_object_path(const char *hash) {
  char *git_root = get_git_root();

  if (git_root == NULL) {
    fprintf(stderr, "fatal: not a git repository (get_git_root): %s\n",
            strerror(errno));
    return NULL;
  }

  char *git_object_path = construct_git_object_path(git_root, hash);

  if (git_object_path) {
    if (access(git_object_path, F_OK) != 0) {
      free(git_root);
      free(git_object_path);
      fprintf(stderr, "fatal: Not a valid object name %s\n", hash);
      return NULL;
    }
    free(git_root);
    return git_object_path;
  }

  free(git_root);
  return NULL;
}

char *mk_git_object_path(const char *hash) {
  char *git_root = get_git_root();

  if (git_root == NULL) {
    fprintf(stderr, "fatal: not a git repository (get_git_root): %s\n",
            strerror(errno));
    return NULL;
  }

  char *git_object_path = construct_git_object_path(git_root, hash);

  if (git_object_path) {
    free(git_root);
    return git_object_path;
  }

  free(git_root);
  return NULL;
}

long copy_bytes(FILE *output_stream, FILE *input_stream) {
  long bytes = 0;
  int c;
  while ((c = getc(input_stream)) != EOF) {
    bytes++;
    if (fputc(c, output_stream) < 0) {
      perror("fputc");
      return -1;
    }
  }

  // Check if we stopped due to error or genuine EOF
  if (ferror(input_stream)) {
    perror("getc");
    return -1;
  }

  return bytes;
}
