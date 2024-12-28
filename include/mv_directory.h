#ifndef MV_DIRECTORY_H
#define MV_DIRECTORY_H

#include <sys/types.h> // Gerekli türler için
#include <sys/stat.h>  // struct stat için
#include <dirent.h>    // struct dirent için
#include <limits.h>    // PATH_MAX için
#include <stdio.h>     // printf, perror için
#include <stdlib.h>    // exit için
#include <string.h>    // strcmp, snprintf için

// Fonksiyon prototipi
int mv_directory(const char* source, const char* destination);

#endif // MV_DIRECTORY_H