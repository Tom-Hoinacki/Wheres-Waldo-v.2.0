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

//TODO: change to local variable for memory clearing
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
void breadth_first_create_first_parent_linked_list(char * dirPathLvl1, dirLvlList * parentCurr, dirLvlList * parentHead);
void allocate_mem_linked_lists_and_paths(void);
void store_dir_path_to_make_in_node_iterate_next(char * dirPath, int i);
void reset_to_parent_head_free_currDir_mem(void);
void allocate_and_iterate_to_next_child_node(void);
void store_parent_path_to_next_child_node(void);
void assign_child_node_to_parent_node(void);
void transpose_childpath_to_parentpath(void);
void iterate_to_next_node_in_lists(void);





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
    strncpy(WALDO_DIR_NAME_FORMAT, "%s/Level %d-%d", PATH_MAX);
    
    const char * WALDO_FILE_NAME_FORMAT = (char *)malloc(strlen(1 + "%s/File %d.txt"));
    strncpy(WALDO_FILE_NAME_FORMAT, "%s/File %d.txt", PATH_MAX);
    
    
    allocate_mem_linked_lists_and_paths();
    
    
    /*******    DIRECTORY LEVEL SEARCH LOOP (OUTERMOST LOOP)    ********/
    /*******************************************************************/
    
    /* Search for Waldo sightings by using a parent and child directory level linked lists to map out the
     * text file order to search through and log sightings. Each linked list item holds the text file path to search for sightings.
     *
     * Iterate through each parent linked list item that holds the text file path to log sightings into log file, while also creating
     * child directory level linked list items that store the text file paths for each child text file of the current parent linked list
     * item. This newly assigned child directory level linked list will be used in the next iteration.
     *
     * At end of each parent directory level sightings log iteration, assign entire child level linked list items to
     * replace entire parent level linked list in order to prepare for next search sightings log iteration.
     */
    
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
            breadth_first_create_first_parent_linked_list(dirPathLvl1, parentCurr, parentHead);
        }

        
        
        //*******    CURRENT DIRECTORY LEVEL FOLDER AND FILE CREATION LOOP    *********//
        /*******************************************************************************/
        
        /* Iterate through parent linked list while logging Waldo sightings in each directory's text files
         * Create and assign child directory level linked list to
         * parent linked list before next directory level iteration of logging sightings
         /************************************************************************************/
        
        firstChild = 1;
        int childCount = 0;
        
        while (dirLevelCount > 0)
        {
            
            // Open current directory
            d = opendir(parentCurr->path);
            stat(parentCurr->path, &st);
            
            
            // Loop through folders and files in directory
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
                // Else add directory to child level linked list
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
                        allocate_and_iterate_to_next_child_node();
                    }
                    
                    // If there are any children directories, make linked list node, store parent's path
                    logNextLevel = 1;
                    
                    store_parent_path_to_next_child_node();
                    
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
        
        // Set number of child directories to be made in the next level
        dirLevelCount = childCount;
        
        // Assign child directory level linked list to parent level linked list before
        // next level iteration of directory creations
        while (childCount > 0)
        {
            if (firstChild)
            {
                parentHead = parentCurr;
                firstChild = 0;
            }
            
            assign_child_node_to_parent_node();
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


void breadth_first_create_first_parent_linked_list(char * dirPathLvl1, dirLvlList * parentCurr, dirLvlList * parentHead)
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
            store_dir_path_to_make_in_node_iterate_next(dirPathLvl1, i++);
        }
    }
    
    reset_to_parent_head_free_currDir_mem();
}


/* Saves directory path for next node to create */
void store_dir_path_to_make_in_node_iterate_next(char * dirPath, int i)
{
    char * tempDirPath = malloc(PATH_MAX);
    strncpy(tempDirPath, dirPath, PATH_MAX);
    strcat(tempDirPath, "/");
    strcat(tempDirPath, in_Dir->d_name);
    
    parentCurr->path = malloc(PATH_MAX);
    strncpy(parentCurr->path, tempDirPath, PATH_MAX);
    
    
    // If first element store in head
    if (i == 0)
    {
        parentHead = parentCurr;
    }
    
    parentCurr->next = (dirLvlList *)malloc(sizeof(*parentCurr->next));
    parentCurr = parentCurr->next;
    
    dirLevelCount++;
}

void reset_to_parent_head_free_currDir_mem()
{
    // reset head and free memory
    parentCurr = parentHead;
    free(in_Dir);
    free(d);
    in_Dir = NULL;
    d = NULL;
   
}

//TODO: change to single line strncpy child to parent
void store_parent_path_to_next_child_node()
{
    tempDirPath = (char *)malloc(PATH_MAX);
    newDirPath = (char *)malloc(PATH_MAX);
    
    strncpy(tempDirPath, parentCurr->path, PATH_MAX);
    //TODO: try change strcat to strncat
    strcat(tempDirPath, "/");
    strcat(tempDirPath, in_Dir->d_name);
    
    // Assign parent folder path
    strncpy(newDirPath,tempDirPath, PATH_MAX);
    
    char * cPath = (char *)malloc(PATH_MAX);
    strncpy(cPath, newDirPath, PATH_MAX);
    
    childCurr->path = (char *)malloc(PATH_MAX);
    strncpy(childCurr->path, cPath, PATH_MAX);
    
    // Free temp char pointer to path string
    free(newDirPath);
    free(tempDirPath);
    free(cPath);
}

void allocate_and_iterate_to_next_child_node()
{
    childCurr->next = (dirLvlList *)malloc(sizeof(*childCurr->next));
    childCurr->next->path = (char *)malloc(PATH_MAX);
    childCurr = childCurr->next;
}

void assign_child_node_to_parent_node()
{
    // Transpose child path to parent path
    transpose_childpath_to_parentpath();
   
    // Initialize new next parent node
    parentCurr->next = (dirLvlList *)malloc(sizeof(*parentCurr->next));
    parentCurr->next->path = (char *)malloc(PATH_MAX);
    
    // Point to next element in parent and child level linked lists
    iterate_to_next_node_in_lists();
}

void transpose_childpath_to_parentpath()
{
    char * tmpPath = (char *)malloc(PATH_MAX);
    strcpy(tmpPath, childCurr->path);
    
    char * childToParentTemp = (char *)malloc(PATH_MAX);
    strncpy(childToParentTemp, tmpPath, PATH_MAX);
    parentCurr->path = (char *)malloc(PATH_MAX);
    strncpy(parentCurr->path, childToParentTemp, PATH_MAX);
    
    free(tmpPath);
    tmpPath = NULL;
    free(childToParentTemp);
}

void iterate_to_next_node_in_lists()
{
    parentCurr = parentCurr->next;
    
    if (parentCurr->path == NULL)
    {
        parentCurr->path = (char *)malloc(PATH_MAX);
    }
    
    childCurr = childCurr->next;
}



