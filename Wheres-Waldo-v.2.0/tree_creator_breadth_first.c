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
        const int WALDO_DIR_NAME_LEN = 11;
        
        char * const WALDO_DIR_NAME_FORMAT = (char *)malloc(strlen(1 + "%s/Level %d-%d"));
        strcpy(WALDO_DIR_NAME_FORMAT, "%s/Level %d-%d");
        
        char * const WALDO_FILE_NAME_FORMAT = (char *)malloc(strlen(1 + "%s/File %d.txt"));
        strcpy(WALDO_FILE_NAME_FORMAT, "%s/File %d.txt");
    

        // Seed random number generator
        srand(time(NULL));
    
    
        // Initialize children directory level counters for how many children directory
        //left to create at that level (random 1-3 for Level 2, random 0-3 for Levels 3 & 4)
        int mkDirCount = rand() % 3 + 1;
    
        int lvlNum = 2;// Initilaize number for first child directory level 2, used for creating path strings
        int createNextLevel = 1; // Initialize flag for whether to create next level down
        int dirNum; // Initialize number of current child directory being created
        char * newDirPath; // Initialize new directory path
        char * newFilePath;  // Initialize new file path

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
    
        parentCurr = (dirLvlList *)malloc(sizeof(dirLvlList));
        parentHead = (dirLvlList *)malloc(sizeof(dirLvlList));
    
        childCurr = (dirLvlList *)malloc(sizeof(dirLvlList));
        childHead = (dirLvlList *)malloc(sizeof(dirLvlList));
    
    
    
        //*******    DIRECTORY LEVEL CREATION LOOP (OUTERMOST LOOP)    *********//
        /*******************************************************************/
    
        /* Create child directory levels using a parent and child level linked lists
         * Create parent directory level folders while making a parent linked list
         * Iterate through each parent linked list element to add to child directory level linked list
         * Once parent directory done creating folders, files, and child directory level
         * linked list, then resassign parent linked list with child linked list before
         * next iteration */
        while (createNextLevel == 1)
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
                    
                    // Assign parent folder path
                    newDirPath = (char *)malloc(strlen(1 + dirPathLvl1 + WALDO_DIR_NAME_LEN));
                    sprintf(newDirPath, WALDO_DIR_NAME_FORMAT, dirPathLvl1, lvlNum, dirNum++);
                    parentCurr->path = newDirPath;
                    
                    // Free temp char pointer to path string
                    free(newDirPath);
                    
                    
                    // If not last folder, initialize new linked list element
                    if ((mkDirCount - i) > 1)
                    {
                        parentCurr->next = (dirLvlList *)malloc(sizeof(dirLvlList));
                        
                        // If first element store in head
                        if (i == 0)
                        {
                            parentHead = parentCurr;
                        }
                        
                        parentCurr = parentCurr->next;
                    }
                    else
                    {
                        parentCurr->next = NULL;
                    }
                }
            }
         
            
 
            
            
             //*******    CURRENT DIRECTORY LEVEL FOLDER AND FILE CREATION LOOP    *********//
             /*******************************************************************************/
    
            
            /* Iterate through parent linked list creating directory and its text files
             * create and assign child directory level linked list to 
             * parent linked list before next level iteration of directory and text file creation
            /************************************************************************************/
            while (parentCurr != NULL)
            {
                
                // Get random num 1-3 and create that number of empty text files in new directory
                newFilePath = malloc(PATH_MAX);
                
                // Create parent directory
                mkdir(parentCurr->path, 0700);
                
                // Create rand 3 files, write to with text filler file and random "Waldo" insertions
                create_rand3_file_num(newFilePath, parentCurr->path, loremIpsumFilePath, WALDO_FILE_NAME_FORMAT);
                
                
                
                // Created child directory level linked list until reaching last level(4)
                if (lvlNum < 4)
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
    
                        // If there are any children to create must check to make next level
                        createNextLevel = 1;
                        
                        // Get random 0-3 grandchildren directories to be created
                        childCurr->childDirToCreate = random() % 4;
                        
                        // Assign path for child directory creation in next directory level iteration
                        newDirPath = (char *)malloc(PATH_MAX);
                        strcpy(newDirPath, parentCurr->path);
                        sprintf(newDirPath, WALDO_DIR_NAME_FORMAT, newDirPath, (lvlNum + 1), dirNum++);
                        childCurr->path = newDirPath;
                        free(newDirPath);
                       
                       
                        
                        // If first iteration assign child head as first element in list
                        if (dirNum == 2)
                        {
                            childHead = childCurr;
                        }
                        
                        // If any children elements left to be created, initialize, allocate, point to next newly created list element
                        if (parentCurr->childDirToCreate > 0)
                        {
                            childCurr->next = (dirLvlList *)malloc(sizeof(dirLvlList));
                            childCurr = childCurr->next;
                        }
                        
                        parentCurr->childDirToCreate--;
                    }
                }
                
                // Point to next element in parent level linked list
                parentCurr = parentCurr->next;
            }
            
            
            
            //*******    TRANSPOSE CHILD LINKED LIST TO PARENT LINKED LIST    *********//
            //*******    LIST USED TO CREATE NEXT LEVEL FOLDERS AND FILES     *********//
            /***************************************************************************/

            
            // Set linked lists to point to first element
            parentCurr = parentHead;
            childCurr = childHead;
    
            // Assign child directory level linked list to parent level linked list before
            // next level iteration
            while (childCurr != NULL)
            {
                parentCurr->childDirToCreate = childCurr->childDirToCreate;
                parentCurr->path = childCurr->path;
                
                // If the next parent element does not already exist, initialize, allocate memory
                if (parentCurr->next == NULL)
                {
                    parentCurr->next = (dirLvlList *)malloc(sizeof(dirLvlList));
                }

                // Point to next element in parent and child level linked lists
                parentCurr = parentCurr->next;
                childCurr = childCurr->next;
            }
            
            // Set parent linked list to first element to prepare for creating directories in next level iteration
            parentCurr = parentHead;
            
            
            // Free memory of child linked list
            struct list_elm * next_child_elm = childHead;
            
            while (next_child_elm != NULL)
            {
                next_child_elm = childHead->next;
                free(childHead);
                childHead = next_child_elm;
            }
            
            
            // Reinitizalie child link list current and head, allocate memory for current directory level
            childCurr = (dirLvlList *)malloc(sizeof(dirLvlList));
            childHead = (dirLvlList *)malloc(sizeof(dirLvlList));
    
            lvlNum++;
        }
    
    
    // Free parent and child linked list memory
    free(childCurr);
    free(childHead);
    
    struct list_elm * next_parent_elm = parentHead;
    
    while (next_parent_elm != NULL)
    {
        //next_parent_elm = parentHead->next;
        free(parentHead);
        parentHead = next_parent_elm;
    }
    
    
    // Free variable pointer memory
    free(WALDO_DIR_NAME_FORMAT);
    free(WALDO_FILE_NAME_FORMAT);
    free(newFilePath);
}





    
    
    
    
    
    
    
    
    