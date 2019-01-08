#ifndef PROJECT4_H
#define PROJECT4_H

#include <iostream>
#include <unistd.h>
#include <fstream>
#include "parser.h"
#include "token.h"
#include "testTree.h"
#include "node.h"
#include "semantics.h"

using namespace std;

/* Prototypes */
int fileToDataArray(string, char *);
int getFileSize(string);
void printPreorder(struct Node*);

#endif
