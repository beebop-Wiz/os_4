#include <stdlib.h>
#include <sys/call.h>
#include <unistd.h>
#include <string.h>

void *malloc(size_t sz) {
  return (void *) syscall(2, sz, 0, 0);
}

void exit(int err) {
  syscall(1, err, 0, 0);
}

int setenv(const char *envname, const char *envval, int overwrite) {
  char **temp_environ = environ;
  char *temp_envp, name[128];
  size_t i, l, n = 0;
  for(temp_envp = *temp_environ; temp_envp; temp_envp = *temp_environ++, n++) {
    l = strlen(temp_envp);
    for(i = 0; i < l && i < 128 && temp_envp[i] != '='; i++) name[i] = temp_envp[i];
    name[i] = 0;
    if(!strcmp(name, envname)) break;
  }
  n--;
  if(!temp_envp) {
    temp_environ = environ;
    temp_environ = malloc((n + 1) * sizeof(char *));
    for(i = 0; i < l; i++) {
      temp_environ[i] = malloc(strlen(environ[i]) + 1);
      memcpy(temp_environ[i], environ[i], strlen(environ[i]));
    }
    environ = temp_environ;
    environ[n] = malloc(strlen(envname) + strlen(envval) + 2);
    memcpy(environ[n], envname, strlen(envname));
    environ[n][strlen(envname)] = '=';
    memcpy(environ[n] + strlen(envname) + 1, envval, strlen(envval));
    environ[n][strlen(envname) + strlen(envval) + 1] = 0;
    environ[n+1] = 0;
  } else if(overwrite) {
    environ[n] = malloc(strlen(envname) + strlen(envval) + 2);
    memcpy(environ[n], envname, strlen(envname));
    environ[n][strlen(envname)] = '=';
    memcpy(environ[n] + strlen(envname) + 1, envval, strlen(envval));
    environ[n][strlen(envname) + strlen(envval) + 1] = 0;
  }
  return 0;
}

char *getenv(const char *envname) {
  char **temp_environ = environ;
  char *temp_envp, name[128];
  size_t i, l;
  for(temp_envp = *temp_environ; temp_envp; temp_envp = *temp_environ++) {
    l = strlen(temp_envp);
    for(i = 0; i < l && i < 128 && temp_envp[i] != '='; i++) name[i] = temp_envp[i];
    name[i] = 0;
    if(!strcmp(name, envname)) return temp_envp + i + 1;
  }
  return 0;
}
