//
//  sightings_log.c
//  Waldo v1.2
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

#include "sightings_log.h"

//Function Signatures
void log_waldo_sightings_dir_breadth_first(char * dirPath, FILE * sightingsLogFile, int * sightingsCount);
void log_waldo_sightings_dir_depth_first(char * dirPath, FILE * sightingsLogFile, int * sightingsCount);
void log_waldo_sightings_txtfile(char* dirPath, struct dirent* in_File, FILE * sightingsLogFile, int * sightingsCount);


//http://www.macs.hw.ac.uk/~rjp/Coursewww/Cwww/linklist.html
struct list_elm {
    int childDirToCreate;
    char * path;
    struct list_elm * next;
};

typedef struct list_elm dirLvlList;


//Function Implementations


/* LOG SIGHTINGS BREADTH-FIRST IN RANDOM ASYMMETRICAL DIRECTORY TREE */
/*********************************************************************/
void log_waldo_sightings_dir_breadth_first(char * dirPathLvl1, FILE * sightingsLogFile, int * sightingsCount)
{
    //*******    LOCAL VARIABLES    *********//
    /*****************************************/
    
    // Initialize directory path variables
    const int WALDO_DIR_NAME_LEN = 13;
    
    char * const WALDO_DIR_NAME_FORMAT = (char *)malloc(strlen(1 + "%s/Level %d-%d"));
    strcpy(WALDO_DIR_NAME_FORMAT, "%s/Level %d-%d");
    
    char * const WALDO_FILE_NAME_FORMAT = (char *)malloc(strlen(1 + "%s/File %d.txt"));
    strcpy(WALDO_FILE_NAME_FORMAT, "%s/File %d.txt");
    
    int dirLevelCount = 0;
    
    int logNextLevel = 1; // Initialize flag for whether to create next level down
    int dirNum; // Initialize number of current child directory being created
    char * newDirPath = (char *)malloc(PATH_MAX);; // Initialize new directory path
    char * tempDirPath = (char *)malloc(PATH_MAX); // Initialize directory path temp
    
    
    // Create linked list pointers
    dirLvlList * parentCurr, * parentHead;
    dirLvlList * childCurr, * childHead;
    dirLvlList * nextElm;
    
    parentCurr = (dirLvlList *)malloc(sizeof(*parentCurr));
    parentHead = (dirLvlList *)malloc(sizeof(*parentHead));
    
    childCurr = (dirLvlList *)malloc(sizeof(*childCurr));
    childHead = (dirLvlList *)malloc(sizeof(*childHead));
    
    int firstLevel = 1;
    int firstChild = 1;
    
    
    nextElm = (dirLvlList *)malloc(sizeof(*nextElm));
    
    
    
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
            // Save child level directory paths into parent link list
            struct dirent * in_Dir;
            DIR * d;
            struct stat st;
            
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
                
                // If file, open file, start string search, else traverse to next child directory
                if (S_ISDIR(st.st_mode))
                {
                    char * childDirPath = (char *)malloc(PATH_MAX);
                    strcpy(childDirPath, dirPathLvl1);
                    strcat(childDirPath, "/");
                    strcat(childDirPath, in_Dir->d_name);
                    
                    parentCurr->path = (char *)malloc(PATH_MAX);
                    parentCurr->path = childDirPath;
                    
                    
                    // If first element store in head
                    if (i == 0)
                    {
                        parentHead = parentCurr;
                        char * pHeadPath = (char *)malloc(PATH_MAX);
                        strcpy(pHeadPath, childDirPath);
                        parentHead->path = pHeadPath;
                        i++;
                        
                        //logNextLevel = 1;
                    }
                    
                    childDirPath = NULL;
                    free(childDirPath);
                    
                    parentCurr->next = (dirLvlList *)malloc(sizeof(*parentCurr->next) + 1000);
                    parentCurr = parentCurr->next;
                    
                    dirLevelCount++;
                }
                
            }
            
            parentCurr = parentHead;
            
            in_Dir = NULL;
            d = NULL;
            free(in_Dir);
            free(d);
            firstLevel = 0;
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
                        childCurr->next = (dirLvlList *)malloc(sizeof(*childCurr->next) + 10000);
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
         
         childCount = 0;
         firstChild = 1;
         
         
         // Assign child directory level linked list to parent level linked list before
         // next level iteration
         while (childCurr != NULL)
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
             
             
             
            parentCurr->next = (dirLvlList *)malloc(sizeof(*parentCurr->next) + 1000);
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
   // free(newFilePath);
}


void breadth_First_Create_First_Parent_Linked_List(char * dirPathLvl1, dirLvlList * parentCurr, dirLvlList * parentHead)
{
    // Save child level directory paths into parent link list
    struct dirent * in_Dir;
    DIR * d;
    struct stat st;
    
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
        
        // If file, open file, start string search, else traverse to next child directory
        if (S_ISDIR(st.st_mode))
        {
            char * childDirPath = (char *)malloc(PATH_MAX);
            strcpy(childDirPath, dirPathLvl1);
            strcat(childDirPath, "/");
            strcat(childDirPath, in_Dir->d_name);
            
            parentCurr->path = (char *)malloc(PATH_MAX);
            parentCurr->path = childDirPath;
            
            
            // If first element store in head
            if (i == 0)
            {
                parentHead = parentCurr;
                char * pHeadPath = (char *)malloc(PATH_MAX);
                strcpy(pHeadPath, childDirPath);
                parentHead->path = pHeadPath;
                i++;
                
                //logNextLevel = 1;
            }
            
            childDirPath = NULL;
            free(childDirPath);
            
            parentCurr->next = (dirLvlList *)malloc(sizeof(*parentCurr->next) + 1000);
            parentCurr = parentCurr->next;
        }
        
    }
    
    parentCurr = parentHead;
    
    in_Dir = NULL;
    d = NULL;
    free(in_Dir);
    free(d);
}



/* Recursive method to search all text files for "Waldo" string DEPTH-FIRST */
void log_waldo_sightings_dir_depth_first(char * dirPath, FILE * sightingsLogFile, int * sightingsCount)
{
    struct dirent * in_Dir;
    DIR * d;
    struct stat st;
    char * childDirPath = (char *)malloc(PATH_MAX);
    strcpy(childDirPath, dirPath);
    
    // Open current directory
    d = opendir(dirPath);
    stat(dirPath, &st);
    
    // Child directories
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
            log_waldo_sightings_txtfile(dirPath, in_Dir, sightingsLogFile, sightingsCount);
        }
        else if (S_ISDIR(st.st_mode))
        {
            strcat(childDirPath, "/");
            strcat(childDirPath, in_Dir->d_name);
            log_waldo_sightings_dir_depth_first(childDirPath, sightingsLogFile, sightingsCount);
            strcpy(childDirPath, dirPath);
        }
    }
    
    free(childDirPath);
}


/* Opens text file and searches line by line for "Waldo" and prints to log file */
void log_waldo_sightings_txtfile(char* dirPath, struct dirent* in_File, FILE * sightingsLogFile, int * sightingsCount)
{
    char * filePath = malloc(PATH_MAX);
    FILE * entry_File;
    const int BUF_RLINE_SIZE = 150; // Memory allocated for one line
    char lineBuf[BUF_RLINE_SIZE];
    
    strcpy(filePath, dirPath);
    strcat(filePath, "/");
    strcat(filePath, in_File->d_name);
    
    struct stat statbuf;
    
    // Open current text file being navigated
    entry_File = fopen(filePath, "r");
    
    if (entry_File == NULL)
    {
        fprintf(stderr, "Error: Failed to open entry file - %s\n", strerror(errno));
        fclose(entry_File);
    }
    
    // Take lines in 150 character chunks using a buffer to read for "Waldo"
    while (fgets(lineBuf, BUF_RLINE_SIZE, entry_File) != NULL)
    {
        const char * tmp = lineBuf;
        
        // Take a substring up to the point "Waldo" is found in a line, if found print occurence in log
        // file and start searching through the remainder of the line until next "Waldo" and so on...
        while((tmp = strstr(tmp, "Waldo")))
        {
            // Print sighting to log file includes: Sighting occurence #, file path and line number sighted at
            fprintf(sightingsLogFile, "%d:\t%s (Line %c)\n", *sightingsCount, filePath, lineBuf[0]);
            (*sightingsCount)++; // Increment count printed at start of each log file line
            
            // End of line is hit on "Waldo" sighting then break
            if (tmp == "Waldo\n")
                break;
            
            tmp += strlen("Waldo ");
        }
    }
    
    // Close the current text file
    fclose(entry_File);
}


