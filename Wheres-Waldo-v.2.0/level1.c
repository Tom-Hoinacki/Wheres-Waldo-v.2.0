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

//Header Function Signatures
void get_dir_lvl1_Path(char * outputDir, char ** dirPathLvl1, int * pathLen);
void check_to_remove_existing_waldo_directory(char * outputPath);
int remove_directory(const char *path);
void create_log_file(char * dirPathLvl1, char ** logPath, char * logName);
void create_text_filler_file(char * dirPathLvl1, char ** loremIpsumFilePath, int pathLen);
void log_creation_path(FILE * logPath, char * newPath);

//File Function Sigatures
void get_output_path(char * outputDir, char ** dirPathLvl1, char * name,  int * pathLen);
void build_path_string(char ** path, int * len, char * outputDir, char * name);





//Function Implementations

/* Creates Waldo Level 1 folder */
void get_dir_lvl1_Paths(char * outputDir, char ** dirPathLvl1_Breadth, char ** dirPathLvl1_Depth, int * pathLen)
{
    // Create breadth dir level 1 name
    char * dirNameLvl1Breadth = (char *)malloc(strlen(1 + "/Breadth-First Level 1"));
    strcpy(dirNameLvl1Breadth, "/Breadth-First Level 1");
    
    // Get breadth dir level 1 path
    get_output_path(outputDir, dirPathLvl1_Breadth, dirNameLvl1Breadth, pathLen);
    
    
    // Create depth dir level 1 name
    char * dirNameLvl1Depth = (char *)malloc(strlen(1 + "/Depth-First Level 1"));
    strcpy(dirNameLvl1Depth, "/Depth-First Level 1");
    
    // Get depth dir level 1 path
    get_output_path(outputDir, dirPathLvl1_Depth, dirNameLvl1Depth, pathLen);
}

void get_output_path(char * outputDir, char ** dirPathLvl1, char * name,  int * pathLen)
{
    // Assign pathLen pointer for reuse in main to point to len's address
    int len = 1 + strlen(outputDir) + strlen(name);
    *pathLen = len;
    
    // Allocate memory for level 1 folder path, assign, and append to build its string name
    build_path_string(dirPathLvl1, pathLen, outputDir, name);
    
    // Free memory pointer will not be used again
    free(name);
}


void build_path_string(char ** path, int * len, char * outputDir, char * name)
{
    *path = (char *)malloc(*len);
    strcpy(*path, outputDir);
    strcat(*path, name);
}


/* Checks if a Waldo directory tree already exists in same local drive location, if so remove existing directory tree */
void check_to_remove_existing_waldo_directory(char * outputPath)
{
    struct stat st = {0};
    
    // Check if path exists already, if so remove
    if (stat(outputPath, &st) != -1)
    {
        remove_directory(outputPath);
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
void create_log_file(char * dirPathLvl1, char ** logPath, char * logName)
{
    // Assign and append log file path
    strcpy(*logPath, dirPathLvl1);
    strcat(*logPath, logName);
    
    // Create log file with path, close after creation
    FILE * fd = fopen(*logPath, "w+");
    
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


/* Logs creation path order as program builds out the directory trees and their files */
void log_creation_path(FILE * logFile, char * newPath)
{
    // Print creation path to log file
    fprintf(logFile, "%s\n", newPath);
}


/* Create text filler file that lives inside Waldo level and has string Waldo randomly inserted */
void create_text_filler_file(char * waldoOutputPath, char ** loremIpsumFilePath, int pathLen)
{
    // Initialize, allocate memory, assign name string
    char * loremIpsumFileName = (char *)malloc(strlen(1 + "/_Lorem Ipsum Text Filler.txt"));
    strcpy(loremIpsumFileName, "/_Lorem Ipsum Text Filler.txt");
    
    // Allocate memory for text filler file path, build and assign file path
    *loremIpsumFilePath = (char *) malloc(1+ pathLen + strlen(loremIpsumFileName));
    strcpy(*loremIpsumFilePath, waldoOutputPath);
    strcat(*loremIpsumFilePath, loremIpsumFileName);
    
    // Free memory
    free(loremIpsumFileName);
    
    // Initialize, allocate memory, assign filler text string
    char * loremIpsum= (char *)malloc(strlen(1 + "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum")) ;
    
    strcpy(loremIpsum, "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum");
    
    
    // Create read/write file for text filler file
    FILE* fd2= fopen(*loremIpsumFilePath, "w+");
    
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

