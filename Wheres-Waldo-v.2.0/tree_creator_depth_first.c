//
//  tree_creator_depth_first.c
//  Waldo v2.0
//
//  Created by Thomas Hoinacki on 7/16/16.
//  Copyright (c) 2016 Thomas Hoinacki. All rights reserved.
//

// C LIBRARY HEADERS
//******************
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdbool.h>
#include <time.h>
#include <sys/syslimits.h>
#include <fcntl.h>


// APPLICATION HEADERS
//********************
#include "tree_creator_depth_first.h"
#include "file_creator.h"
#include "level1.h"



// FILE SCOPE VARIABLES
//***************

// Initialize traversal path node struct and doubly linked list
//http://c.learncodethehardway.org/book/ex32.html
static struct ListNode;

typedef struct ListNode {
    struct ListNode * next;
    struct ListNode * prev;
    char * parentPath;
} ListNode;

typedef struct List {
    int count;
    ListNode * first;
    ListNode * last;
} List;

static List * depthList;

// Initialize path string holders and formats
static char * newDirPath;
static char * newFilePath;
static char * tempDirPath;

// Initialize top level, max level, and current level
static const int FIRST_CHILD_LVL = 2;
static const int LEVEL_MAX = 4;
static int lvlNum = 2;

// Number of directories left to create at a given level
static int numDirToMakeAtLvl[LEVEL_MAX + 1];

// Current directory number to make next at a given level
static int currDirNumAtLvl[LEVEL_MAX + 1];

// Create log file pointer
static FILE * creationLogFile;



// FUNCTION SIGNATURES
//********************
void create_depth_first_random_asym_dir_tree(char * dirPathLvl1, char * creationLogPath, char * loremIpsumFilePath);
void initialize_traversal_path_linked_list(char * dirPathLvl1);
void initialize_new_path_vars(void);
void init_assign_local_vars_before_tree_creation(char * dirPathLvl1, char * creationLogPath);
void create_new_dir_and_text_files_log_creation(char * textFillerPath, const char * dirNameFormat, const char * fileNameFormat);
void iterate_init_assign_next_path_linked_list_node(void);
void free_memory_and_close_creation_log(FILE * creationLogFile, const char * dirNameFormat, const char * fileNameFormat);
void free_linked_list(List * depthList);
void free_path_and_format_vars(const char * dirNameFormat, const char * fileNameFormat);



// FUNCTION IMPLEMENTATIONS
//*************************

/* CREATE RANDOM ASYMMETRICAL CHILD DIRECTORY TREE
 * RANDOM BETWEEN 2-4 LEVELS OF CHILD DEPTH
 * RANDOM BETWEEN 1-3 CHILD DIRECTORIES CREATED WITHIN EACH DIRECTORY
 * RANDOM BETWEEN 1-3 TEXT FILES IN EACH DIRECTORY CREAT
 * RANDOM 1/100 CHANCE STRING "Waldo" IS INSERTED AFTER EACH WORD STREAMED AND PRINTED INTO EACH TEXT FILE
 /*********************************************************************************************************/
void create_depth_first_random_asym_dir_tree(char * dirPathLvl1, char * creationLogPath, char * loremIpsumFilePath)
{
    // Initialize path formatting vars
    const char * WALDO_DIR_NAME_FORMAT = (char *)malloc(strlen(1 + "%s/Level %d-%d"));
    strcpy(WALDO_DIR_NAME_FORMAT, "%s/Level %d-%d");
    
    const char * WALDO_FILE_NAME_FORMAT = (char *)malloc(strlen(1 + "%s/File %d.txt"));
    strcpy(WALDO_FILE_NAME_FORMAT, "%s/File %d.txt");
    
    
    // Initialize all other local variables
    init_assign_local_vars_before_tree_creation(dirPathLvl1, creationLogPath);
    
    // Seed random number generator for number of directories to be created under each leaf
    srand(time(NULL));
 
    
    /* RECURSIVE DEPTH-FIRST ALGORITHM, FIRST CREATES AT LEAST ONE           */
    /* LEVEL 2 DIRECTORY, RANDOMIZES CREATION OF CHILD DIRECTORIES AND FILES */
    /*************************************************************************/
   
    while (numDirToMakeAtLvl[lvlNum] >= 0)
    {
        // If no dir to create on first child level break, creation of tree is done
        if ((lvlNum == FIRST_CHILD_LVL) && (numDirToMakeAtLvl[lvlNum] == 0))
        {
            break;
        }
        // If no dir to create on current level, go up to parent level if not already at top level
        else if ((lvlNum > FIRST_CHILD_LVL) && (numDirToMakeAtLvl[lvlNum] == 0))
        {
            depthList->last = depthList->last->prev;
            lvlNum--;
            continue;
        }
        
        create_new_dir_and_text_files_log_creation(loremIpsumFilePath, WALDO_DIR_NAME_FORMAT, WALDO_FILE_NAME_FORMAT);
        
        // Check next level won't exceed max
        if ((lvlNum + 1) > LEVEL_MAX)
        {
            // Check if current level has more siblings
            if (numDirToMakeAtLvl[lvlNum] > 0)
            {
                continue;
            }
            // Else go back to parent level
            else
            {
                depthList->last = depthList->last->prev;
                lvlNum--;
            }
        }
        // Check next level is not first or last level
        else if ((lvlNum + 1) > FIRST_CHILD_LVL && (lvlNum + 1) <= LEVEL_MAX)
        {
            // Get how many children direct children directories are to be created
            numDirToMakeAtLvl[++lvlNum] = rand() % 4;
            currDirNumAtLvl[lvlNum] = 1;

            // Check if there are children directories to be made
            if (numDirToMakeAtLvl[lvlNum] > 0)
            {
                iterate_init_assign_next_path_linked_list_node();
            }
            // Else stay to iterate through siblings on current level
            else
            {
                lvlNum--;
            }
        }
        // Check if more siblings to create
        else if (numDirToMakeAtLvl[lvlNum] > 0)
        {
            continue;
        }
        // Else traverse back up to parent directory
        else
        {
            depthList->last = depthList->last->prev;
            lvlNum--;
        }
    }
    
    free_memory_and_close_creation_log(creationLogFile, WALDO_DIR_NAME_FORMAT, WALDO_FILE_NAME_FORMAT);
}



// HELPER FUNCTIONS FOR CREATE TREE ALGORITHM
//******************************************

void init_assign_local_vars_before_tree_creation(char * dirPathLvl1, char * creationLogPath)
{
    // Open creation log file
    creationLogFile = fopen(creationLogPath, "ab+");
    
    // Initialize path string holders and formats
    initialize_new_path_vars();
    
    // Initialize linked list to keep track of parent paths to traverse up directory tree
    initialize_traversal_path_linked_list(dirPathLvl1);
    
    // Number of directories left at corresponding index to level value (random 1-3 for first child level)
    numDirToMakeAtLvl[lvlNum] = rand() % 3 + 1;
    
    // Current directory number to make next at a given level with new path
    currDirNumAtLvl[lvlNum] = 1;
}


void initialize_traversal_path_linked_list(char * dirPathLvl1)
{
    // Allocate memory
    depthList = (List *)malloc(sizeof(*depthList));
    depthList->first = (ListNode *)malloc(sizeof(*depthList->first));
    depthList->last = (ListNode *)malloc(sizeof(*depthList->last));
    depthList->last->parentPath = (char *)malloc(PATH_MAX);
    
    // Set first linked list node
    depthList->first = depthList->last;
    
    // Set current linked list node with level path parameter
    tempDirPath = (char *)malloc(PATH_MAX);
    strcpy(tempDirPath, dirPathLvl1);
    strcpy(depthList->last->parentPath, tempDirPath);
}


void initialize_new_path_vars()
{
    newDirPath = (char *) malloc (PATH_MAX);
    newFilePath = (char *) malloc(PATH_MAX);
}


void create_new_dir_and_text_files_log_creation(char * textFillerPath, const char * dirNameFormat, const char * fileNameFormat)
{
    // Create next directory at current level
    sprintf(newDirPath, dirNameFormat, depthList->last->parentPath, lvlNum, currDirNumAtLvl[lvlNum]++);
    mkdir(newDirPath, 0700);
    
    // Log directory creation path
    log_creation_path(creationLogFile, newDirPath);
    
    numDirToMakeAtLvl[lvlNum]--;
    
    // Create random number (1-3) of filler text files with Waldo randomly inserted under new directory
    create_rand3_file_num(newFilePath, newDirPath, creationLogFile, textFillerPath, fileNameFormat);
}


void iterate_init_assign_next_path_linked_list_node()
{
    char * temp = malloc(PATH_MAX);
    strcpy(temp, newDirPath);
    
    depthList->last->next = (ListNode *)malloc(sizeof(*depthList->last->next));
    depthList->last->next->parentPath = (char *)malloc(PATH_MAX);
    
    // Assign next linked list element with
    strcpy(depthList->last->next->parentPath, temp);
    
    free(temp);
    
    depthList->last->next->prev = depthList->last;
    depthList->last = depthList->last->next;
}


void free_memory_and_close_creation_log(FILE * creationLogFile, const char * dirNameFormat, const char * fileNameFormat)
{
    // Free memory of linked lists
    free_linked_list(depthList);
    
    // Free memory of all pointers used for random asymmetrical directory tree creation
    free_path_and_format_vars(dirNameFormat, fileNameFormat);
    
    // Close creation log file, free pointer
    if (creationLogFile != NULL)
    {
        fclose(creationLogFile);
    }
}


void free_linked_list(List * depthList)
{
    ListNode * nextElm = (ListNode *)malloc(sizeof(*nextElm));
    
    while (nextElm != NULL)
    {
        nextElm = depthList->first->next;
        free(depthList->first->parentPath);
        free(depthList->first);
        depthList->first = nextElm;
    }
    
    free(nextElm);
    free(depthList);
}


void free_path_and_format_vars(const char * dirNameFormat, const char * fileNameFormat)
{
    free(dirNameFormat);
    free(fileNameFormat);
    free(newDirPath);
    free(newFilePath);
}


