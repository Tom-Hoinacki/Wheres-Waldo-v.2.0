//
//  tree_creator_breadth_first.c
//  Wheres-Waldo-v.2.0
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
#include "tree_creator_breadth_first.h"
#include "file_creator.h"


// Function Signatures
void create_breadth_first_random_asym_dir_tree(char * dirPathLvl1, char * loremIpsumFilePath);



// Function Implementations


/* CREATE BREADTH-FIRST RANDOM ASYMMETRICAL CHILD DIRECTORY TREE
 * RANDOM BETWEEN 2-4 LEVELS OF CHILD DEPTH
 * RANDOM BETWEEN 1-3 CHILD DIRECTORIES CREATED WITHIN EACH DIRECTORY
 * RANDOM BETWEEN 1-3 TEXT FILES IN EACH DIRECTORY CREATED
 * RANDOM 1/100 CHANCE STRING "Waldo" IS INSERTED AFTER EACH WORD STREAMED AND PRINTED INTO EACH TEXT FILE
 /*********************************************************************************************************/
void create_breadth_first_random_asym_dir_tree(char * dirPathLvl1, char * loremIpsumFilePath)
{
        //*******    LOCAL VARIABLES    *********//
        /*****************************************/
    
        // Initialize directory path variables
        const int WALDO_DIR_NAME_LEN = 13;
        
        char * const WALDO_DIR_NAME_FORMAT = (char *)malloc(strlen(1 + "%s/Level %d-%d"));
        strcpy(WALDO_DIR_NAME_FORMAT, "%s/Level %d-%d");
        
        char * const WALDO_FILE_NAME_FORMAT = (char *)malloc(strlen(1 + "%s/File %d.txt"));
        strcpy(WALDO_FILE_NAME_FORMAT, "%s/File %d.txt");
    

        // Seed random number generator
        srand(time(NULL));
    
    
        // Initialize children directory level counters for how many children directory
        //left to create at that level (random 1-3 for Level 2, random 0-3 for Levels 3 & 4)
        // int mkDirCount = rand() % 3 + 1;
        int mkDirCount= 3;
        int dirsToCreate = mkDirCount;
    
        int childCounter = 0;
    
    
        int lvlNum = 2;// Initilaize number for first child directory level 2, used for creating path strings
        int createNextLevel = 1; // Initialize flag for whether to create next level down
        int dirNum; // Initialize number of current child directory being created
        char * newDirPath = (char *)malloc(PATH_MAX);; // Initialize new directory path
        char * newFilePath = (char *)malloc(PATH_MAX);;  // Initialize new file path
        char * tempDirPath = (char *)malloc(PATH_MAX); // Initialize directory path temp
    

        // Create empty linked list
        //http://www.macs.hw.ac.uk/~rjp/Coursewww/Cwww/linklist.html
        struct list_elm {
            int childDirToCreate;
            char * path;
            struct list_elm * next;
        };
        
        typedef struct list_elm dirLvlList;
    
        dirLvlList * parentCurr, * parentHead;
        dirLvlList * childCurr, * childHead;
        dirLvlList * nextElm;
    
        parentCurr = (dirLvlList *)malloc(sizeof(*parentCurr));
        parentHead = (dirLvlList *)malloc(sizeof(*parentHead));
    
        childCurr = (dirLvlList *)malloc(sizeof(*childCurr));
        childHead = (dirLvlList *)malloc(sizeof(*childHead));
    
        int firstChild = 1;
    
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
                    //parentCurr->childDirToCreate = random() % 4;
                    parentCurr->childDirToCreate = 3;
                    
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
                
                newDirPath = (char *)malloc(PATH_MAX);
                
                strcpy(newDirPath, parentCurr->path);
                //strcpy(tempDirPath, newDirPath);
                
                // Create rand 3 files, write to with text filler file and random "Waldo" insertions
                create_rand3_file_num(newFilePath, newDirPath, loremIpsumFilePath, WALDO_FILE_NAME_FORMAT);
                
                
                
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
                        //childCurr->childDirToCreate = random() % 4;
                        childCurr->childDirToCreate = 3;
                        
                        
                        
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
                
                char * childToParentTemp = (char *)malloc(PATH_MAX);
                strcpy(childToParentTemp, tempDirPath);
                
                //printf(parentCurr->path, childToParentTemp);
                strcpy(parentCurr->path, childToParentTemp);
               
                
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
                free(childToParentTemp);
                tempDirPath = NULL;
                childToParentTemp = NULL;

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
        free(childHead);
        childHead = nextElm;
    }
    struct list_elm * next_parent_elm = parentHead;
    
    while (next_parent_elm != NULL)
    {
        next_parent_elm = parentHead->next;
        free(parentHead);
        parentHead = next_parent_elm;
    }
    
    
    // Free variable pointer memory
    free(childHead);
    free(WALDO_DIR_NAME_FORMAT);
    free(WALDO_FILE_NAME_FORMAT);
    free(newFilePath);
}





    
    
    
    
    
    
    
    
    