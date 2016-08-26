//
//  tree_creator_breadth_first.c
//  Wheres-Waldo-v.2.0
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
#include "tree_creator_breadth_first.h"
#include "file_creator.h"


// FILE SCOPE VARIABLES
//***************
static FILE * creationLogFile;


static const int WALDO_DIR_NAME_LEN = 13;

// Initialize children directory level counters for how many children directory
//left to create at that level (random 1-3 for Level 2, random 0-3 for Levels 3 & 4)
static int mkDirCount;
static int dirsToCreate;
static int childCounter;

// First child flag
int firstChild = 1;

static int lvlNum = 2;// Initilaize number for first child directory level 2, used for creating path strings
static int createNextLevel = 1; // Initialize flag for whether to create next level down
static int dirNum; // Initialize number of current child directory being created
static char * newDirPath; // Initialize new directory path
static char * newFilePath;  // Initialize new file path
static char * tempDirPath; // Initialize directory path temp


// Create empty linked list
//http://www.macs.hw.ac.uk/~rjp/Coursewww/Cwww/linklist.html
struct list_elm {
    int childDirToCreate;
    char * path;
    struct list_elm * next;
};

typedef struct list_elm dirLvlList;

// Parent and child level linked list node pointers
static dirLvlList * parentCurr, * parentHead;
static dirLvlList * childCurr, * childHead;
static dirLvlList * nextElm;




// FUNCTION SIGNATURES
//********************
void create_breadth_first_random_asym_dir_tree(char * dirPathLvl1, char * creationLogPath, char * loremIpsumFilePath);




// FUNCTION IMPLEMENTATIONS
//*************************

/* CREATE BREADTH-FIRST RANDOM ASYMMETRICAL CHILD DIRECTORY TREE
 * RANDOM BETWEEN 2-4 LEVELS OF CHILD DEPTH
 * RANDOM BETWEEN 1-3 CHILD DIRECTORIES CREATED WITHIN EACH DIRECTORY
 * RANDOM BETWEEN 1-3 TEXT FILES IN EACH DIRECTORY CREATED
 * RANDOM 1/100 CHANCE STRING "Waldo" IS INSERTED AFTER EACH WORD STREAMED AND PRINTED INTO EACH TEXT FILE
 /*********************************************************************************************************/
void create_breadth_first_random_asym_dir_tree(char * dirPathLvl1, char * creationLogPath, char * loremIpsumFilePath)
{
    //*******    LOCAL VARIABLES    *********//
    /*****************************************/
    // Open creation log file
    creationLogFile = fopen(creationLogPath, "ab+");
   
    // Initialize directory path variables
    
    char * const WALDO_DIR_NAME_FORMAT = (char *)malloc(strlen(1 + "%s/Level %d-%d"));
    strcpy(WALDO_DIR_NAME_FORMAT, "%s/Level %d-%d");
    
    char * const WALDO_FILE_NAME_FORMAT = (char *)malloc(strlen(1 + "%s/File %d.txt"));
    strcpy(WALDO_FILE_NAME_FORMAT, "%s/File %d.txt");

    mkDirCount = rand() % 3 + 1;
    dirsToCreate = mkDirCount;
    childCounter = 0;
    
    // Seed random number generator
    srand(time(NULL));

    newDirPath = (char *)malloc(PATH_MAX); // Initialize new directory path
    newFilePath = (char *)malloc(PATH_MAX);;  // Initialize new file path
    tempDirPath = (char *)malloc(PATH_MAX); // Initialize directory path temp
    
    parentCurr = (dirLvlList *)malloc(sizeof(*parentCurr));
    parentHead = (dirLvlList *)malloc(sizeof(*parentHead));
    childCurr = (dirLvlList *)malloc(sizeof(*childCurr));
    childHead = (dirLvlList *)malloc(sizeof(*childHead));
    
    nextElm = (dirLvlList *)malloc(sizeof(*nextElm));
   

    //*******    DIRECTORY LEVEL CREATION LOOP (OUTERMOST LOOP)    *********//
    /*******************************************************************/

    /* Create child directory levels using a parent and child level linked lists
     * Create parent directory level folders while making a parent linked list
     * Iterate through each parent linked list element to add to child directory level linked list
     * Once parent directory done creating folders, files, and child directory level
     * linked list, then resassign parent linked list with child linked list before
     * next iteration */
    while (createNextLevel)
    {
        // Reset directory number to 1 for creating directory path string
        dirNum = 1;
        
        // Set create next level to false unless one of the parents has
        // children to create
        createNextLevel = 0;
       

        //*******    CREATE FIRST PARENT DIRECTORY LEVEL LINKED LIST                         *********//
        //*******    STORES PATHS FOR FOLDER CREATION AND CHILD DIRECTORIES TO BE CREATED    *********//
        /**********************************************************************************************/

        // Create initial parent directory level linked list on first iteration
        // Initialize child head and current to prepare to add to child linked list
        if (lvlNum == 2)
        {
            for (int i = 0; i < mkDirCount; i++)
            {
                // Random get children to create, make current parent directory path and folder
                parentCurr->childDirToCreate = random() % 4;
                
                newDirPath = (char *)malloc(PATH_MAX);
                // Assign parent folder path
                sprintf(newDirPath, WALDO_DIR_NAME_FORMAT, dirPathLvl1, lvlNum, dirNum++);
                
                char * pPath = (char *)malloc(PATH_MAX);
                strcpy(pPath, newDirPath);
                
                parentCurr->path = (char *)malloc(PATH_MAX);
                parentCurr->path = pPath;
                
                // Free temp char pointer to path string
                free(newDirPath);
                
                
                // If not last folder, initialize new linked list element
                if ((mkDirCount - i) > 0)
                {
                    parentCurr->next = (dirLvlList *)malloc(sizeof(*parentCurr->next) + 1000);
                    
                    // If first element store in head
                    if (i == 0)
                    {
                        parentHead = parentCurr;
                        char * pHeadPath = (char *)malloc(PATH_MAX);
                        strcpy(pHeadPath, newDirPath);
                        parentHead->path = pHeadPath;
                    }
                    
                    parentCurr = parentCurr->next;
                }
                else
                {
                    parentCurr->next = NULL;
                }
            }
            
            parentCurr = parentHead;
        }
     
        
        lvlNum++;
        firstChild = 1;
        
        
         //*******    CURRENT DIRECTORY LEVEL FOLDER AND FILE CREATION LOOP    *********//
         /*******************************************************************************/

        
        /* Iterate through parent linked list creating directory and its text files
         * create and assign child directory level linked list to 
         * parent linked list before next level iteration of directory and text file creation
        /************************************************************************************/
        while (dirsToCreate > 0)
        {
            
            // Get random num 1-3 and create that number of empty text files in new directory
            newFilePath = malloc(PATH_MAX);
            
            // Create parent directory
            mkdir(parentCurr->path, 0700);
            
            // Log directory creation path
            log_creation_path(creationLogFile, parentCurr->path);
            
            newDirPath = (char *)malloc(PATH_MAX);
            
            strcpy(newDirPath, parentCurr->path);
            //strcpy(tempDirPath, newDirPath);
            
            // Create rand 3 files, write to with text filler file and random "Waldo" insertions
            create_rand3_file_num(newFilePath, newDirPath, creationLogFile, loremIpsumFilePath, WALDO_FILE_NAME_FORMAT);
            
            
            
            // Created child directory level linked list until reaching last level(4)
            if (lvlNum <= 4)
            {
                
                
                //*******    CREATE AND STORE CHILD DIRECTORY LINKED LIST FOR NEXT ITERATION    *********//
                /*****************************************************************************************/
    
                
                /* Iterate through number of child directories to be
                 * added to child directory level linked list with path & number of grandchildren
                 * to be made, will assign parent linked list with child link list before
                 * iterating the level directory creation process again */
                
                // Reset directory number for each next set of sibling child folders to be made
                dirNum = 1;
                
                while (parentCurr->childDirToCreate > 0)
                {
                    // If first iteration assign child head as first element in list
                    if (firstChild)
                    {
                        childHead = childCurr;
                        firstChild = 0;
                    }
                    else
                    {
                        childCurr->next = (dirLvlList *)malloc(sizeof(*childCurr->next) + 10000);
                        childCurr = childCurr->next;

                    }

                    // If there are any children to create must check to make next level
                    createNextLevel = 1;
                    
                    // Get random 0-3 grandchildren directories to be created
                    childCurr->childDirToCreate = random() % 4;
                    //childCurr->childDirToCreate = 3;
                    
                    newDirPath = (char *)malloc(PATH_MAX);
                    tempDirPath = (char *)malloc(PATH_MAX);
                    strcpy(tempDirPath, parentCurr->path);
                    
                    
                    // Assign parent folder path
                    sprintf(newDirPath, WALDO_DIR_NAME_FORMAT, tempDirPath, lvlNum, dirNum++);
                    
                    char * cPath = (char *)malloc(PATH_MAX);
                    strcpy(cPath, newDirPath);
                    
                    childCurr->path = (char *)malloc(PATH_MAX);
                    strcpy(childCurr->path, cPath);
                  
                    
                    // Free temp char pointer to path string
                    free(newDirPath);
                    free(tempDirPath);
                    free(cPath);
                    
                    newDirPath = NULL;
                    tempDirPath = NULL;
                    cPath = NULL;
                    
                    childCounter++;
                   
                    parentCurr->childDirToCreate--;
                }
            }
            
            // Point to next element in parent level linked list
            parentCurr = parentCurr->next;
            dirsToCreate--;
        }
        
            
        
        //*******    TRANSPOSE CHILD LINKED LIST TO PARENT LINKED LIST    *********//
        //*******    LIST USED TO CREATE NEXT LEVEL FOLDERS AND FILES     *********//
        /***************************************************************************/

        
        // Set linked lists to point to first element
        parentCurr = parentHead;
        childCurr = childHead;
        
        // Set number of child directories to be made in the next level
        dirsToCreate = childCounter;
        
        childCounter = 0;
        firstChild = 1;


        // Assign child directory level linked list to parent level linked list before
        // next level iteration
        while (childCurr != NULL)
        {
            parentCurr->childDirToCreate = childCurr->childDirToCreate;
            
            tempDirPath = (char *)malloc(PATH_MAX);
            strcpy(tempDirPath, childCurr->path);
            
            char * swapTemp = (char *)malloc(PATH_MAX);
            strcpy(swapTemp, tempDirPath);
            strcpy(parentCurr->path, swapTemp);
           
            if (firstChild)
            {
                parentHead = parentCurr;
                firstChild = 0;
            }
            
            parentCurr->next = (dirLvlList *)malloc(sizeof(*parentCurr->next) + 1000);
            
            
            // Point to next element in parent and child level linked lists
            parentCurr = parentCurr->next;
            
            if (parentCurr->path == NULL)
            {
                parentCurr->path = (char *)malloc(PATH_MAX);
            }
            
            childCurr = childCurr->next;

            
            free(tempDirPath);
            free(swapTemp);
            tempDirPath = NULL;
            swapTemp = NULL;
        }
        
        // Set parent linked list to first element to prepare for creating directories in next level iteration
        parentCurr = parentHead;
        childCurr = childHead;
        firstChild = 1;
    }
    
    
    // Free parent and child linked list memory
    nextElm = childHead;
    
    while (nextElm != NULL)
    {
        nextElm = childHead->next;
        free(childHead->path);
        free(childHead);
        childHead = nextElm;
    }
    struct list_elm * next_parent_elm = parentHead;
    
    while (next_parent_elm != NULL)
    {
        next_parent_elm = parentHead->next;
        free(parentHead->path);
        free(parentHead);
        parentHead = next_parent_elm;
    }
    
    
    // Free variable pointer memory
    free(childHead);
    free(parentHead);
    free(WALDO_DIR_NAME_FORMAT);
    free(WALDO_FILE_NAME_FORMAT);
    free(newFilePath);
    
    // Close creation log file, free point
    if (creationLogFile != NULL)
    {
        fclose(creationLogFile);
    }
}





    
    
    
    
    
    
    
    
    