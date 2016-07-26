//
//  breadth_sightings.c
//  Wheres-Waldo-v.2.0
//
//  Created by Thomas Hoinacki on 7/26/16.
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
#include "breadth_sightings.h"
#include "sightings_log.h"


// FILE SCOPE VARIABLES
//***************

// Linked list for parent and children levels
//http://www.macs.hw.ac.uk/~rjp/Coursewww/Cwww/linklist.html
static struct list_elm {
    int childDirToCreate;
    char * path;
    struct list_elm * next;
};

typedef struct list_elm dirLvlList;

// Create linked list pointers to point to parent and child level lists
static dirLvlList * parentCurr, * parentHead;
static dirLvlList * childCurr, * childHead;
static dirLvlList * nextElm;


// Path name format vars
static const int WALDO_DIR_NAME_LEN = 13;

static int dirLevelCount = 0;
static int logNextLevel = 1; // Initialize flag for whether to create next level down
static int dirNum; // Initialize number of current child directory being created
static char * newDirPath; // Initialize new directory path
static char * tempDirPath; // Initialize directory path temp
static char * childDirPath; // Initialize child directory path


// Flags to mark if iteration is on first child / leve
static int firstLevel = 1;
static int firstChild = 1;

// Pointers for directories and files
static struct dirent * in_Dir;
static DIR * d;
static struct stat st;



// FUNCTION SIGNATURES
//********************
void log_waldo_sightings_dir_breadth_first(char * dirPath, FILE * sightingsLogFile, int * sightingsCount);
void breadth_First_Create_First_Parent_Linked_List(char * dirPathLvl1, dirLvlList * parentCurr, dirLvlList * parentHead);
void allocate_mem_linked_lists_and_paths(void);
void store_dir_path_to_make_in_node_iterate_next(char * dirPath, int i);
void reset_to_parent_head_free_currDir_mem(void);




// FUNCTION IMPLEMENTATIONS
//*************************

/* LOG SIGHTINGS BREADTH-FIRST IN RANDOM ASYMMETRICAL DIRECTORY TREE */
/*********************************************************************/
void log_waldo_sightings_dir_breadth_first(char * dirPathLvl1, FILE * sightingsLogFile, int * sightingsCount)
{
    //*******    LOCAL VARIABLES    *********//
    /*****************************************/
    
    // Initialize directory path variables
    const char * WALDO_DIR_NAME_FORMAT = (char *)malloc(strlen(1 + "%s/Level %d-%d"));
    strcpy(WALDO_DIR_NAME_FORMAT, "%s/Level %d-%d");
    
    const char * WALDO_FILE_NAME_FORMAT = (char *)malloc(strlen(1 + "%s/File %d.txt"));
    strcpy(WALDO_FILE_NAME_FORMAT, "%s/File %d.txt");
    
    
    allocate_mem_linked_lists_and_paths();
    
    
    /*******    DIRECTORY LEVEL SEARCH LOOP (OUTERMOST LOOP)    ********/
    /*******************************************************************/
    
    /* Search child directory levels using a parent and child level linked lists
     * Search parent directory level folders while making a parent linked list
     * Iterate through each parent linked list element to log sightings and child directories to
     * child level linked list
     * Once parent directory is done logging sightings from its texts files,
     * Assign child link list to parent list link list before next iteration */
    while (logNextLevel)
    {
        // Set log next level to false, unless child level found
        logNextLevel = 0;
        
        
        /*******    CREATE FIRST PARENT DIRECTORY LEVEL LINKED LIST                               *********/
        /*******    STORES PATHS FOR LOG SIGHTINGS SEARCH AND CHILD DIRECTORIES TO BE SEARCHED    *********/
        /**************************************************************************************************/
        
        // Create initial parent directory level linked list on first iteration
        // Initialize child head and current to prepare to add to child linked list
        if (firstLevel)
        {
            breadth_First_Create_First_Parent_Linked_List(dirPathLvl1, parentCurr, parentHead);
        }

        
        
        //*******    CURRENT DIRECTORY LEVEL FOLDER AND FILE CREATION LOOP    *********//
        /*******************************************************************************/
        
        /* Iterate through parent linked list logging sightings in each directory's text files
         * Create and assign child directory level linked list to
         * parent linked list before next directory level iteration of logging sightings
         /************************************************************************************/
        
        firstChild = 1;
        int childCount = 0;
        
        while (dirLevelCount > 0)
        {
            // Iterate all text files in current directory and log sightings
            struct dirent * in_Dir;
            DIR * d;
            struct stat st;
            
            
            // Open current directory
            d = opendir(parentCurr->path);
            stat(parentCurr->path, &st);
            
            // Log sightings in text files, then add children directory paths to child level linked list
            while ((in_Dir = readdir(d)) != NULL)
            {
                if (strcmp(in_Dir->d_name, ".") == 0 || strcmp(in_Dir->d_name, "..") == 0)
                    continue;
                
                if (fstatat(dirfd(d), in_Dir->d_name, &st, 0) < 0)
                {
                    perror(in_Dir->d_name);
                    continue;
                }
                
                // If file, open file, start string search, else traverse to next child directory
                if (S_ISREG(st.st_mode))
                {
                    log_waldo_sightings_txtfile(parentCurr->path, in_Dir, sightingsLogFile, sightingsCount);
                }
                // Esle add directory to child level linked list
                else if (S_ISDIR(st.st_mode))
                {
                    // If first iteration assign child head as first element in list
                    if (firstChild)
                    {
                        childHead = childCurr;
                        firstChild = 0;
                    }
                    else
                    {
                        childCurr->next = (dirLvlList *)malloc(sizeof(*childCurr->next));
                        childCurr = childCurr->next;
                    }
                    
                    // If there are any children to create must check to make next level
                    logNextLevel = 1;
                    
                    
                    newDirPath = (char *)malloc(PATH_MAX);
                    tempDirPath = (char *)malloc(PATH_MAX);
                    strcpy(tempDirPath, parentCurr->path);
                    strcat(tempDirPath, "/");
                    strcat(tempDirPath, in_Dir->d_name);
                    
                    
                    // Assign parent folder path
                    strcpy(newDirPath,tempDirPath);
                    // sprintf(newDirPath, WALDO_DIR_NAME_FORMAT, tempDirPath, lvlNum, dirNum++);
                    
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
                    
                    childCount++;
                    
                    parentCurr->childDirToCreate--;
                }
            }
            
            dirLevelCount--;
            parentCurr = parentCurr->next;
        }
        
        
        //*******    TRANSPOSE CHILD LINKED LIST TO PARENT LINKED LIST         *********//
        //*******    LIST USED TO LOG SIGHTINGS FROM NEXT LEVEL TEXT FILES     *********//
        /***************************************************************************/
        
        
        // Set linked lists to point to first element
        parentCurr = parentHead;
        childCurr = childHead;
        
        // Set number of child directories to be made in the next level
        dirLevelCount = childCount;
        firstChild = 1;
        
        // Assign child directory level linked list to parent level linked list before
        // next level iteration
        while (childCount > 0)
        {
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
            
            
            parentCurr->next = (dirLvlList *)malloc(sizeof(*parentCurr->next));
            parentCurr->next->path = (char *)malloc(PATH_MAX);
            
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
            childCount--;
        }
        
        // Set parent linked list to first element to prepare for creating directories in next level iteration
        parentCurr = parentHead;
        childCurr = childHead;
        
        firstChild = 1;
    }
    
    
    // Free parent and child linked list memory
//    nextElm = childHead;
//    
//    while (nextElm != NULL)
//    {
//        nextElm = childHead->next;
//        free(childHead->path);
//        free(childHead);
//        childHead = nextElm;
//    }
//    
//    nextElm = parentHead;
//    
//    while (nextElm != NULL)
//    {
//       nextElm = parentHead->next;
//        free(parentHead);
//        parentHead = nextElm;
//    }
    
//    ListNode * nextElm = (ListNode *)malloc(sizeof(*nextElm));
//    
//    while (nextElm != NULL)
//    {
//        nextElm = depthList->first->next;
//        free(depthList->first->parentPath);
//        free(depthList->first);
//        depthList->first = nextElm;
//    }
//    
//    free(nextElm);
//    free(depthList);
    
    // Free variable pointer memory
    free(childHead);
    free(parentHead);
    free(WALDO_DIR_NAME_FORMAT);
    free(WALDO_FILE_NAME_FORMAT);
}


void allocate_mem_linked_lists_and_paths()
{
    newDirPath = (char *)malloc(PATH_MAX);;
    tempDirPath = (char *)malloc(PATH_MAX);
    childDirPath = (char *)malloc(PATH_MAX);
    
    // Node memory for linked lists, hold path and children to create for each directory
    parentCurr = (dirLvlList *)malloc(sizeof(*parentCurr));
    parentHead = (dirLvlList *)malloc(sizeof(*parentHead));
    childCurr = (dirLvlList *)malloc(sizeof(*childCurr));
    childHead = (dirLvlList *)malloc(sizeof(*childHead));
    nextElm = (dirLvlList *)malloc(sizeof(*nextElm));
}


void breadth_First_Create_First_Parent_Linked_List(char * dirPathLvl1, dirLvlList * parentCurr, dirLvlList * parentHead)
{
    int i = 0;
    
    // Open current directory
    d = opendir(dirPathLvl1);
    stat(dirPathLvl1, &st);
    
    // Iterate through child directories
    while ((in_Dir = readdir(d)) != NULL)
    {
        if (strcmp(in_Dir->d_name, ".") == 0 || strcmp(in_Dir->d_name, "..") == 0)
            continue;
        
        if (fstatat(dirfd(d), in_Dir->d_name, &st, 0) < 0)
        {
            perror(in_Dir->d_name);
            continue;
        }
        
        // If dir, store path for new directory to be created in parent linked list node,
        // then allocate memory to next parent node and iterate to next
        if (S_ISDIR(st.st_mode))
        {
            store_dir_path_to_make_in_node_iterate_next(dirPathLvl1, i);
        }
    }
    
    reset_to_parent_head_free_currDir_mem();
}

void store_dir_path_to_make_in_node_iterate_next(char * dirPath, int i)
{
    strcpy(tempDirPath, dirPath);
    strcat(tempDirPath, "/");
    strcat(tempDirPath, in_Dir->d_name);
    
    parentCurr->path = tempDirPath;
    
    
    // If first element store in head
    if (i == 0)
    {
        parentHead = parentCurr;
        char * pHeadPath = (char *)malloc(PATH_MAX);
        strcpy(pHeadPath, tempDirPath);
        parentHead->path = pHeadPath;
        free(pHeadPath);
        i++;
    }
    
    parentCurr->next = (dirLvlList *)malloc(sizeof(*parentCurr->next));
    parentCurr = parentCurr->next;
}

void reset_to_parent_head_free_currDir_mem()
{
    // reset head and free memory
    parentCurr = parentHead;
    in_Dir = NULL;
    d = NULL;
    free(in_Dir);
    free(d);
}

