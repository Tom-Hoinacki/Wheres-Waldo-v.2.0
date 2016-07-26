//
//  tree_creator_depth_first.c
//  Waldo v2.0
//
//  Created by Thomas Hoinacki on 7/16/16.
//  Copyright (c) 2016 Thomas Hoinacki. All rights reserved.
//

// C Library Headers
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


// Application Headers
#include "tree_creator_depth_first.h"
#include "file_creator.h"
#include "level1.h"


// Function Signatures
void create_depth_first_random_asym_dir_tree(char * dirPathLvl1, char * creationLogPath, char * loremIpsumFilePath);
void create_rand3_file_num(char * newFilePath, char * newDirPath, FILE * creationLogFile,  char * loremPath, char * const format);



//Function Implementations

/* CREATE RANDOM ASYMMETRICAL CHILD DIRECTORY TREE
 * RANDOM BETWEEN 2-4 LEVELS OF CHILD DEPTH
 * RANDOM BETWEEN 1-3 CHILD DIRECTORIES CREATED WITHIN EACH DIRECTORY
 * RANDOM BETWEEN 1-3 TEXT FILES IN EACH DIRECTORY CREAT
 * RANDOM 1/100 CHANCE STRING "Waldo" IS INSERTED AFTER EACH WORD STREAMED AND PRINTED INTO EACH TEXT FILE
 /*********************************************************************************************************/
void create_depth_first_random_asym_dir_tree(char * dirPathLvl1, char * creationLogPath, char * loremIpsumFilePath)
{
    // Open creation log file
    FILE * creationLogFile = fopen(creationLogPath, "ab+");
    
    // Initialize children directory path variables
    const int WALDO_DIR_NAME_LEN = 11;
    
    char * const WALDO_DIR_NAME_FORMAT = (char *)malloc(strlen(1 + "%s/Level %d-%d"));
    strcpy(WALDO_DIR_NAME_FORMAT, "%s/Level %d-%d");
    
    char * const WALDO_FILE_NAME_FORMAT = (char *)malloc(strlen(1 + "%s/File %d.txt"));
    strcpy(WALDO_FILE_NAME_FORMAT, "%s/File %d.txt");
    
    char * parentDirName = (char *) malloc(strlen(WALDO_DIR_NAME_FORMAT));
    char * newDirPath = (char *) malloc (PATH_MAX);
    char * newFilePath = (char *) malloc(PATH_MAX);
    
    // Initialize traversal path linked list
    //http://c.learncodethehardway.org/book/ex32.html
    struct ListNode;
    
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

    ListNode * currDir;
    List * depthList;
    
    // Initilize linked list to keep track of parent paths to traverse up directory tree
    depthList = (List *)malloc(sizeof(*depthList));
    depthList->first = (ListNode *)malloc(sizeof(*depthList->first));
    depthList->last = (ListNode *)malloc(sizeof(*depthList->last));
    depthList->last->parentPath = (char *)malloc(PATH_MAX);
    
    // Set first linked list node
    depthList->first = depthList->last;
    
    // Set current linked list node with level path parameter
    char * tempDirPath = (char *)malloc(PATH_MAX);
    strcpy(tempDirPath, dirPathLvl1);
    strcpy(depthList->last->parentPath, tempDirPath);

    // Initialize top level, max level, and current level
    const int FIRST_CHILD_LVL = 2;
    const int LEVEL_MAX = 4;
    int lvlNum = 2;
    
    // Seed random number generator
    srand(time(NULL));
    
    // Number of directories left at corresponding index to level value
    int numDirToMakeAtLvl[LEVEL_MAX + 1];
    numDirToMakeAtLvl[lvlNum] = rand() % 3 + 1;
    
    // Current directory number to make next at a given level
    int currDirNumAtLvl[10];
    currDirNumAtLvl[lvlNum] = 1;
    
    // Initialize number of files left to create in a child directory (random 1-3)
    int filesToCreate;
    int fileNum;
    
   

    
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
        
        // If no dir to create on current level, go up to parent level if the
        if ((lvlNum > FIRST_CHILD_LVL) && (numDirToMakeAtLvl[lvlNum] == 0))
        {
            depthList->last = depthList->last->prev;
            lvlNum--;
            continue;
        }
        
        // Create level 3 directory
        sprintf(newDirPath, WALDO_DIR_NAME_FORMAT, depthList->last->parentPath, lvlNum, currDirNumAtLvl[lvlNum]++);
        mkdir(newDirPath, 0700);
        
        // Log directory creation path
        log_creation_path(creationLogFile, newDirPath);
        
        numDirToMakeAtLvl[lvlNum]--;
        
        // Get random num 1-3 and create that number of empty text files in new directory
        create_rand3_file_num(newFilePath, newDirPath, creationLogFile, loremIpsumFilePath, WALDO_FILE_NAME_FORMAT);
        
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
               // currDir = depthList->last->prev;
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
            currDir = depthList->last->prev;
            depthList->last = depthList->last->prev;
            lvlNum--;
        }
    }
    
    // Free memory of linked lists
    ListNode *nextElm = (ListNode *)malloc(sizeof(*nextElm));
//    
//    while (nextElm != NULL)
//    {
//        nextElm = depthList->first->next;
//        //free(depthList->first->parentPath);
//        free(depthList->first);
//        depthList->first = nextElm;
//    }
//    struct list_elm * next_parent_elm = parentHead;
//    
//    while (next_parent_elm != NULL)
//    {
//        next_parent_elm = parentHead->next;
//        free(parentHead->path);
//        free(parentHead);
//        parentHead = next_parent_elm;
//    }
//    

    
    // Free memory of all pointers used for random asymmetrical directory tree creation
    free(WALDO_DIR_NAME_FORMAT);
    free(WALDO_FILE_NAME_FORMAT);
    free(parentDirName);
    free(newDirPath);
    free(newFilePath);
    
    // Close creation log file, free pointer
    if (creationLogFile != NULL)
    {
        fclose(creationLogFile);
    }
}



