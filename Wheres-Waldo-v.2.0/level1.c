//
//  level1.c
//  Waldo v1.2
//
//  Created by Thomas Hoinacki on 7/14/16.
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

// Application Header
#include "level1.h"

//Function Signatures
void get_dir_lvl1_Path(char * cwd, char ** dirPathLvl1, int * pathLen);
void check_to_remove_existing_waldo_directory(char * dirPathLvl1);
int remove_directory(const char *path);
void create_sightings_log_file(char * dirPathLvl1, char ** sightingsLogPath, int pathLen);
void create_text_filler_file(char * dirPathLvl1, char ** loremIpsumFilePath, int pathLen);



//Function Implementations

/* Creates Waldo Level 1 folder */
void get_dir_lvl1_Path(char * cwd, char ** dirPathLvl1, int * pathLen)
{
    // Initialize, allocate memory, assign name string
    char * dirNameLvl1 = (char *)malloc(strlen(1 + "/Where's Waldo Level 1"));
    strcpy(dirNameLvl1, "/Where's Waldo Level 1");
    
    // Assign pathLen pointer for reuse in main to point to len's address
    int len = 1 + strlen(cwd) + strlen(dirNameLvl1);
    *pathLen = len;
    
    // Allocate memory for level 1 folder path, assign, and append to build its string name
    *dirPathLvl1 = (char *) malloc(*pathLen);
    strcpy(*dirPathLvl1, cwd);
    strcat(*dirPathLvl1, dirNameLvl1);
    
    // Free memory pointer will not be used again
    free(dirNameLvl1);
}



/* Checks if a Waldo directory tree already exists in same local drive location, if so remove existing directory tree */
void check_to_remove_existing_waldo_directory(char * dirPathLvl1)
{
    struct stat st = {0};
    
    // Check if path exists already, if so remove
    if (stat(dirPathLvl1, &st) != -1)
    {
        remove_directory(dirPathLvl1);
    }
}


/* Recursively removes depth first a directory and all its child directories and files */
//http://stackoverflow.com/questions/2256945/removing-a-non-empty-directory-programmatically-in-c-or-c
int remove_directory(const char *path)
{
    DIR *d = opendir(path);
    size_t path_len = strlen(path);
    int r = -1;
    
    if (d)
    {
        struct dirent *p;
        
        r = 0;
        
        // While child directory exists keep looping and traversing depth-first across directory tree
        while (!r && (p=readdir(d)))
        {
            int r2 = -1;
            char *buf;
            size_t len;
            
            /* Skip the names "." and ".." as we don't want to recurse on them. */
            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
            {
                continue;
            }
            
            len = path_len + strlen(p->d_name) + 2;
            buf = malloc(len);
            
            if (buf)
            {
                struct stat statbuf;
                
                snprintf(buf, len, "%s/%s", path, p->d_name);
                
                // Check if child path exists in file directory before taking action
                if (!stat(buf, &statbuf))
                {
                    // if path is a child directory recursively call method to keep traversing down tree
                    if (S_ISDIR(statbuf.st_mode))
                    {
                        r2 = remove_directory(buf);
                    }
                    // else it is a file and not a directory, remove the file
                    else
                    {
                        r2 = unlink(buf);
                    }
                }
                
                // Free memory taken by directory/file path created to check for
                free(buf);
            }
            
            /* Assign before next iteration to check if there are any more children directories or files in current directory
             will stay as -1 as when initialized at top of loop when there are no more files or directories to remove,
             ending the loop */
            r = r2;
        }
        
        closedir(d);
    }
    
    if (!r)
    {
        r = rmdir(path);
    }
    
    return r;
}



/* Create Waldo sightings log file */
void create_sightings_log_file(char * dirPathLvl1, char ** sightingsLogPath, int pathLen)
{
    // Initialize, allocate memory, assign name string
    char * sightingsLogName = (char *)malloc(strlen(1 + "/Waldo Sightings Log.txt"));
    strcpy(sightingsLogName, "/Waldo Sightings Log.txt");
    
    // Allocate memory for sightings file path, build and assign file path
    *sightingsLogPath = (char *) malloc(1 + pathLen + strlen(sightingsLogName));
    strcpy(*sightingsLogPath, dirPathLvl1);
    strcat(*sightingsLogPath, sightingsLogName);
    
    // Free memory
    free(sightingsLogName);
    
    // Create log file with path, close after creation
    FILE* fd = fopen(*sightingsLogPath, "ab+");
    
    // Close file
    if(fd != NULL)
    {
        fclose(fd);
    }
    else
    {
        int error = strerror(errno);
    }
}


/* Create text filler file that lives inside Waldo level and has string Waldo randomly inserted */
void create_text_filler_file(char * dirPathLvl1, char ** loremIpsumFilePath, int pathLen)
{
    // Initialize, allocate memory, assign name string
    char * loremIpsumFileName = (char *)malloc(strlen(1 + "/Waldo Lorem Ipsum Filler.txt"));
    strcpy(loremIpsumFileName, "/Waldo Lorem Ipsum Filler.txt");
    
    // Allocate memory for text filler file path, build and assign file path
    *loremIpsumFilePath = (char *) malloc(1+ pathLen + strlen(loremIpsumFileName));
    strcpy(*loremIpsumFilePath, dirPathLvl1);
    strcat(*loremIpsumFilePath, loremIpsumFileName);
    
    // Free memory
    free(loremIpsumFileName);
    
    // Initialize, allocate memory, assign filler text string
    char * loremIpsum= (char *)malloc(strlen(1 + "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum")) ;
    
    strcpy(loremIpsum, "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum");
    
    
    // Create read/write file for text filler file
    FILE* fd2= fopen(*loremIpsumFilePath, "ab+");
    
    // Write filler text to file
    fprintf(fd2, loremIpsum);
    
    // Free text filler string pointer
    free(loremIpsum);
    
    // Close file
    if (fd2 != NULL)
    {
        fclose(fd2);
    }
    else
    {
        int error = strerror(errno);
    }
}

