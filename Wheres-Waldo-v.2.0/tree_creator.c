//
//  tree_creator.c
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


// Application Headers
#include "tree_creator.h"
#include "file_creator.h"


// Function Signatures
void create_random_asym_dir_tree(char * dirPathLvl1, char * loremIpsumFilePath);
void create_rand3_file_num(char * newFilePath, char * newDirPath, char * loremPath, char * const format);



//Function Implementations

/* CREATE RANDOM ASYMMETRICAL CHILD DIRECTORY TREE
 * RANDOM BETWEEN 2-4 LEVELS OF CHILD DEPTH
 * RANDOM BETWEEN 1-3 CHILD DIRECTORIES CREATED WITHIN EACH DIRECTORY
 * RANDOM BETWEEN 1-3 TEXT FILES IN EACH DIRECTORY CREATED
 * RANDOM 1/100 CHANCE STRING "Waldo" IS INSERTED AFTER EACH WORD STREAMED AND PRINTED INTO EACH TEXT FILE
 /*********************************************************************************************************/
void create_random_asym_dir_tree(char * dirPathLvl1, char * loremIpsumFilePath)
{
    
    // Initialize children directory path variables
    const int WALDO_DIR_NAME_LEN = 11;
    
    char * const WALDO_DIR_NAME_FORMAT = (char *)malloc(strlen(1 + "%s/Level %d-%d"));
    strcpy(WALDO_DIR_NAME_FORMAT, "%s/Level %d-%d");
    
    char * const WALDO_FILE_NAME_FORMAT = (char *)malloc(strlen(1 + "%s/File %d.txt"));
    strcpy(WALDO_FILE_NAME_FORMAT, "%s/File %d.txt");
    
    char * parentDirName = (char *) malloc(strlen(WALDO_DIR_NAME_FORMAT));
    char * newDirPath = (char *) malloc (PATH_MAX);
    char * newFilePath = (char *) malloc(PATH_MAX);
    char * dirPathLvl2 = (char *) malloc(1 + strlen(dirPathLvl1) + WALDO_DIR_NAME_LEN);
    
    
    // Initialize children directory level path identity numbers
    int lvlNum = 2;
    
    // Initialize children directory level path identity numbers
    int dirNumLvl2 = 1;
    int dirNumLvl3 = 1;
    int dirNumLvl4 = 1;
    
    // Seed random number generator
    srand(time(NULL));
    
    // Initialize children directory level counters for how many children directory left to create at that level (random 1-3)
    int mkDirLvl2 = rand() % 3 + 1;
    int mkDirLvl3;
    int mkDirLvl4;
    
    // Initialize number of files left to create in a child directory (random 1-3)
    int filesToCreate;
    int fileNum;
    
    // Initialize parent directory path
    char * parentPath = (char *) malloc(PATH_MAX);
    strcpy(parentPath, dirPathLvl1);
    
    
    
    /* RECURSIVE DEPTH-FIRST ALGORITHM, FIRST CREATES AT LEAST ONE LEVEL 2 DIRECTORY, RANDOMIZES CREATION OF CHILD DIRECTORIES AND FILES */
    /*************************************************************************************************************************************/
    
    
    // While there directories left to be made keep creating
    while (mkDirLvl2 > 0 || mkDirLvl3 > 0 || mkDirLvl4 > 0)
    {
        // Switch to create directory at specified level
        switch (lvlNum)
        {
                // Waldo Level 2 Directories
            case 2:
                
                // If level 2 directory is to be created
                if (mkDirLvl2 > 0)
                {
                    // Create new level 2 directory
                    strcpy(parentPath, dirPathLvl1);
                    sprintf(newDirPath, WALDO_DIR_NAME_FORMAT, parentPath, lvlNum, dirNumLvl2++);
                    mkdir(newDirPath, 0700);
                    
                    // Get random num 1-3 and create that number of empty text files in new directory
                    create_rand3_file_num(newFilePath, newDirPath, loremIpsumFilePath, WALDO_FILE_NAME_FORMAT);
                    
                    // Save new parent path for next level down directory creation
                    strcpy(parentPath, newDirPath);
                    
                    mkDirLvl2--;
                    
                    // Get number of directories to create for next level down
                    mkDirLvl3 = rand() % 4;
                    
                    // Find which level is the next to have a directory be created, specify that level to navigate to for next loop iteration
                    if (mkDirLvl3 > 0)
                    {
                        lvlNum = 3;
                        dirNumLvl3 = 1;
                        
                        // Save parent path for directory level 3
                        strcpy(dirPathLvl2, newDirPath);
                    }
                    else if (mkDirLvl2 > 0)
                    {
                        lvlNum = 2;
                    }
                }
                
                break;
                
                // Waldo Level 3 Directories
            case 3:
                
                if (mkDirLvl3 > 0)
                {
                    // Create level 3 directory
                    sprintf(newDirPath, WALDO_DIR_NAME_FORMAT, parentPath, lvlNum, dirNumLvl3++);
                    mkdir(newDirPath, 0700);
                    
                    mkDirLvl3--;
                    
                    // Get random num 1-3 and create that number of empty text files in new directory
                    create_rand3_file_num(newFilePath, newDirPath, loremIpsumFilePath, WALDO_FILE_NAME_FORMAT);
                    
                    // Get how many children direct children directories are to be created
                    mkDirLvl4 = rand() % 4;
                    
                    
                    // Check if there are children directories to be made
                    if (mkDirLvl4 > 0)
                    {
                        lvlNum = 4;
                        
                        // Save parent directory path
                        strcpy(parentPath, newDirPath);
                    }
                    // Check if there are any sibling directories to be made
                    else if (mkDirLvl3 > 0)
                    {
                        lvlNum = 3;
                    }
                    // Else traverse back up to parent directory
                    else
                    {
                        lvlNum = 2;
                    }
                }
                
                break;
                
                // Waldo Level 4 Directories
            case 4:
                
                dirNumLvl4 = 1;
                
                // Since it is the last directory level, loop and create directories until complete
                while (mkDirLvl4 > 0)
                {
                    sprintf(newDirPath, WALDO_DIR_NAME_FORMAT, parentPath, lvlNum, dirNumLvl4++);
                    mkdir(newDirPath, 0700);
                    
                    mkDirLvl4--;
                    
                    // Get random num 1-3 and create that number of empty text files in new directory
                    create_rand3_file_num(newFilePath, newDirPath, loremIpsumFilePath, WALDO_FILE_NAME_FORMAT);
                }
                
                
                // Check if the parent directory has any siblings that need to be created
                if (mkDirLvl3 > 0)
                {
                    lvlNum = 3;
                    strcpy(parentPath, dirPathLvl2);
                }
                // Parent has no siblings left to be create recurse back to grandparent
                else
                {
                    lvlNum = 2;
                }
                
                break;
        }
    }
    
    // Free memory of all pointers used for random asymmetrical directory tree creation
    free(WALDO_DIR_NAME_FORMAT);
    free(WALDO_FILE_NAME_FORMAT);
    free(parentDirName);
    free(newDirPath);
    free(newFilePath);
    free(dirPathLvl2);
}



