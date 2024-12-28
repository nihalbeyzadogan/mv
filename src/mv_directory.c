#include "include/mv_directory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <libgen.h>
#include <limits.h>
#include "mv_file.h"

int mv_directory(const char* source, const char* destination) {
  char dest_path[PATH_MAX];
  snprintf(dest_path, PATH_MAX, "%s/%s", destination, basename((char *)source));

  if (mkdir(dest_path, 0755) != 0) { // burada dir olusturuyoruz sayilar da permissionlari belirtio
    perror("mkdir");
    return 1;
  }

  DIR *dir = opendir(source); //openladik dir'i eger olmazsa error donecek
  if (!dir) {
    perror("opendir");
    return 1;
  }

  struct dirent *entry; //dirent librarysini kullanarak directorynin icine erisiyoruz (tanimli)
  char src_path[PATH_MAX];
  while((entry = readdir(dir)) != NULL) {
    if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) { //her dir icinde . ve .. oluyor onlari atlamasi icin var
      continue;
  }

  snprintf(src_path, PATH_MAX, "%s/%s", source, entry->d_name);

  struct stat st;
  if(stat(src_path, &st) == 0) {
    if(S_ISDIR(st.st_mode)) {
      if(mv_directory(src_path, dest_path) != 0) {
        closedir(dir);
        return 1;
      }
  }else{
    if(mv_file(src_path, dest_path) != 0) {
      closedir(dir);
      return 1;
    }
    }
    }
    }


  closedir(dir); //sonunda dir'i kapatioz
  if(rmdir(source) != 0) {
    perror("rmdir");
    return 1;
  }
  return 0;
}
