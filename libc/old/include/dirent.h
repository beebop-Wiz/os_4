#ifndef DIRENT_H
#define DIRENT_H

struct dirent {
  char d_name[256];
};

struct DIR_t {
  int fd;
};

typedef struct DIR_t DIR;

#define NAME_MAX 256
int alphasort(const struct dirent **, const struct dirent **);
int closedir(DIR *);
int dirfd(DIR *);
DIR *fdopendir(int);
DIR *opendir(const char *);
struct dirent *readdir(DIR *);
int readdir_r(DIR *, struct dirent *, struct dirent **);
void rewinddir(DIR *);
int scandir(const char *, struct dirent ***, int (*)(const struct dirent *), int (*)(const struct dirent **, const struct dirent **));

#endif
