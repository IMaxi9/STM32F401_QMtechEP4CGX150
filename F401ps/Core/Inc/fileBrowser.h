/*
 * fileBrowser.h
 *
 *  Created on: 2 dec 2022.
 *      Author: Maksim Ilyin (imax9.narod.ru)
 */

#ifndef INC_FILEBROWSER_H_
#define INC_FILEBROWSER_H_
#include <stdbool.h>
#include "main.h"

extern myListEntry myFileList[64];
extern char tableName[64*(128)];

#define EVENT_NOTHING 0
#define EVENT_KEYPRESS 1

// the keycodes produced by the core (lower 5bits)
#define KEY_UP  30
#define KEY_DOW 29
#define KEY_LFT 27
#define KEY_RGT 23
#define KEY_RET 15
#define KEY_NOTHING 31
#define KEY_A   0
#define KEY_B   1
#define KEY_C   2
#define KEY_D   3
#define KEY_E   4
#define KEY_F   5
#define KEY_G   6
#define KEY_H   7
#define KEY_I   8
#define KEY_J   9
#define KEY_K   10
#define KEY_L   11
#define KEY_M   12
#define KEY_N   13
#define KEY_O   14
#define KEY_P   16
#define KEY_Q   17
#define KEY_R   18
#define KEY_S   19
#define KEY_T   20
#define KEY_U   21
#define KEY_V   22
#define KEY_W   24
#define KEY_X   25
#define KEY_Y   26
#define KEY_Z   28

// the maximum number of files per directory that can be sorted (in this version, the number of lines displayed)
#define MAX_SORTED_FILES 8

#define PATH_LEN 512
#define MAX_DIRECTORY_DEPTH 15

// the maximum of extensions that can be parsed (separated by "/") (context: filtering files in navigateMenu)
#define MAX_PARSED_EXTENSIONS 10

// the maximum length of extensions (including ".")
#define MAX_LENGTH_EXTENSION 5

void showList(uint8_t showCurrent, uint8_t showFirst);
uint8_t countFiles(void);
void changeDir();
char mapKeyToChar( int key );
char mapCharToKey( char key );
bool fileBrowser( char* extension, bool canAbort );
void setCurrentKey(char* selectedFile);
void setDirectorySize();
void setCoreRoot();
void setLastPath();
void setCorePath();
void changeToCorePath();

// prototypes
void prepareExtensions( char *extension, char extensions[MAX_PARSED_EXTENSIONS][MAX_LENGTH_EXTENSION], int *totalExtensions);

// options to the file browser

#endif /* INC_FILEBROWSER_H_ */
