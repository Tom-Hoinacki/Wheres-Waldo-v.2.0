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
        
        typedef struct list_elm parentLvlDirList;
        typedef struct list_elm childLvlDirList;
    
        parentLvlDirList * parentCurr, * parentHead;
        childLvlDirList * childCurr, * childHead;
    
    
    
    
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
           
    
            // Create initial parent directory level linked list on first iteration
            if (lvlNum == 2)
            {
                for (int i = 0; i < mkDirCount; mkDirCount--)
                {
                    // If first iteration save head positions
                    if (i == 0)
                    {
                        parentCurr = (parentLvlDirList *)malloc(1000);
                        parentHead = parentCurr;
                
                        childCurr = (childLvlDirList *)malloc(1000);
                        childHead = childCurr;
                    }
                    
                    // Random get children to create, make current parent directory path and folder
                    parentCurr->childDirToCreate = random() % 4;
                    
                    // Assign parent folder path
                    newDirPath = (char *)malloc(strlen(1 + dirPathLvl1 + WALDO_DIR_NAME_LEN));
                    sprintf(newDirPath, WALDO_DIR_NAME_FORMAT, dirPathLvl1, lvlNum, dirNum++);
                    parentCurr->path = newDirPath;
                    
                    // Create folder with path
                    mkdir(parentCurr->path, 0700);
                    
                    // Free temp char pointer to path string
                    free(newDirPath);
                    
                    
                    // If not last folder, initialize new linked list element
                    if (mkDirCount > 1)
                    {
                        parentCurr->next = (parentLvlDirList *)malloc(1000);
                    }
                    else
                    {
                        parentCurr->next = NULL;
                    }
                   
                    parentCurr = parentCurr->next;
                }
                
                // Point to first parent element in list again to prepare for second loop
                parentCurr = parentHead;
            }
         
           
    
            // Iterate through parent linked list creating text files
            // and the child directory level linked list
            while (parentCurr != NULL)
            {
                // Reset directory number for each next set of sibling child folders to be made
                dirNum = 1;
                
                // Get random num 1-3 and create that number of empty text files in new directory
                newFilePath = malloc(PATH_MAX);
                
                // If after Level 2, now use parent linked list to created directories based on element saved path
                if (lvlNum > 2)
                {
                    // Create parent directory
                    mkDir(parentCurr->path, 0700);
                }
              
                
                // Create rand 3 files, write to with text filler file and random "Waldo" insertions
                create_rand3_file_num(newFilePath, parentCurr->path, loremIpsumFilePath, WALDO_FILE_NAME_FORMAT);
                
                
                
                // Created child directory level linked list until reaching last level(4)
                if (lvlNum < 4)
                {
                    // Create parent level directory
                    mkdir(parentCurr->path, 0700);
                    
                    
                    /* Iterate through number of child directories to be
                     * added to child directory level linked list with path & number of grandchildren
                     * to be made, will assign parent linked list with child link list before
                     * iterating the level directory creation process again */
                    int i = 0;
                    
                    while (parentCurr->childDirToCreate > 0)
                    {
                        i++;
                        
                        // If first iteration assign child head as first element in list
                        if (i == 1)
                        {
                            childHead = childCurr;
                        }
                        
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
                       
                        // Initialize, allocate, point to next newly created list element
                        childCurr->next = (childLvlDirList *)malloc(1000);
                        childCurr = childCurr->next;
                        
                        parentCurr->childDirToCreate--;
                    }
                }
                
                // Point to next element in parent level linked list
                parentCurr = parentCurr->next;
            }
            
            
            // Set linked lists to point to first element
            parentCurr = parentHead;
            childCurr = childHead;
    
            // Assign child directory level linked list to parent level linked list before
            // next level iteration
            while (childCurr->path != NULL)
            {
                parentCurr->childDirToCreate = childCurr->childDirToCreate;
                parentCurr->path = childCurr->path;
                
                // Point to next element in parent and child level linked lists
                parentCurr->next = (parentLvlDirList *)malloc(sizeof(parentLvlDirList));
                parentCurr = parentCurr->next;
                
                childCurr = childCurr->next;
            }
            
            // Set parent linked list to first element
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
            childCurr = (childLvlDirList *)malloc(sizeof(childLvlDirList));
            childHead = childCurr;
    
            lvlNum++;
        }
    
    
    // Free parent and child linked list memory
    free(childCurr);
    
    struct list_elm * next_parent_elm = parentHead;
    
    while (next_parent_elm != NULL)
    {
        next_parent_elm = parentHead->next;
        free(parentHead);
        parentHead = next_parent_elm;
    }
    
    
    // Free variable pointer memory
    free(WALDO_DIR_NAME_FORMAT);
    free(WALDO_FILE_NAME_FORMAT);
    free(newFilePath);
}





    
    
    
    
    
    
    
    
    