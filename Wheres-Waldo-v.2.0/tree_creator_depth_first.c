//
//  tree_creator_depth_first.c
//  Waldo v2.0
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
#include "tree_creator_depth_first.h"
#include "file_creator.h"
#include "level1.h"


// Function Signatures
void create_depth_first_random_asym_dir_tree(char * dirPathLvl1, char * creationLogPath, char * loremIpsumFilePath);
void create_rand3_file_num(char * newFilePath, char * newDirPath, FILE * creationLogFile,  char * loremPath, char * const format);



//Function Implementations

/* CREATE RANDOM ASYMMETRICAL CHILD DIRECTORY TREE
 * RANDOM BETWEEN 2-4 LEVELS OF CHILD DEPTH
 * RANDOM BETWEEN 1-3 CHILD DIRECTORIES CREATED WITHIN EACH DIRECTORY
 * RANDOM BETWEEN 1-3 TEXT FILES IN EACH DIRECTORY CREAT
 * RANDOM 1/100 CHANCE STRING "Waldo" IS INSERTED AFTER EACH WORD STREAMED AND PRINTED INTO EACH TEXT FILE
 /*********************************************************************************************************/
void create_depth_first_random_asym_dir_tree(char * dirPathLvl1, char * creationLogPath, char * loremIpsumFilePath)
{
    // Open creation log file
    FILE * creationLogFile = fopen(creationLogPath, "ab+");
    
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
    
//    typedef enum
//    {
//        FIRST_CHILD_LEVEL,
//        MIDDLE_LEVEL,
//        LAST_LEVEL
//    } levelType;

    //levelType level = FIRST_CHILD_LEVEL;
    const int FIRST_CHILD_LVL = 2;
    const int LEVEL_MAX = 4;
    const int TOTAL_CHILD_LEVELS = 3;
    int lvlNum = 2;
    
    // Initialize children directory level path identity numbers
    int firstChildLevel = 1;
    int middleLevel = 1;
    int lastLevel = 1;
    
    // Seed random number generator
    srand(time(NULL));
    
    
    // Number of directories left at corresponding index to level value
    int numDirToMakeAtLvl[10];
    numDirToMakeAtLvl[lvlNum] = rand() % 3 + 1;
    
    
    // Current directory number to make next at a given level
    int currDirNumAtLvl[10];
    currDirNumAtLvl[lvlNum] = 1;

    
    // Initialize number of files left to create in a child directory (random 1-3)
    int filesToCreate;
    int fileNum;
    
    // Initialize parent directory path
    char * parentPath = (char *) malloc(PATH_MAX);
    strcpy(parentPath, dirPathLvl1);
    
    char * grandParentPath = (char *) malloc(PATH_MAX);
    
    
    
    /* RECURSIVE DEPTH-FIRST ALGORITHM, FIRST CREATES AT LEAST ONE           */
    /* LEVEL 2 DIRECTORY, RANDOMIZES CREATION OF CHILD DIRECTORIES AND FILES */
    /*************************************************************************/
    
    
    // While there directories left to be made keep creating
//    while (moreDirToCreate)
//    {
//        moreDirToCreate = 0;
    
        // Switch to create directory at specified level
//        switch (level)
//        {
//                // Waldo Level 2 Directories
//            case FIRST_CHILD_LEVEL:
//                
//                // If level 2 directory is to be created
//                if (numDirToMakeAtLvl[lvlNum] > 0)
//                {
//                    // Create new level 2 directory
//                    strcpy(parentPath, dirPathLvl1);
//                    sprintf(newDirPath, WALDO_DIR_NAME_FORMAT, parentPath, lvlNum, currDirNumAtLvl[lvlNum]++);
//                    mkdir(newDirPath, 0700);
//                    
//                    // Log directory creation path
//                    log_creation_path(creationLogFile, newDirPath);
//                    
//                    // Get random num 1-3 and create that number of empty text files in new directory
//                    create_rand3_file_num(newFilePath, newDirPath, creationLogFile, loremIpsumFilePath, WALDO_FILE_NAME_FORMAT);
//                    
//                    // Save new parent path for next level down directory creation
//                    strcpy(parentPath, newDirPath);
//                    
//                    numDirToMakeAtLvl[lvlNum]--;
//                    
//                    // Get number of directories to create for next level down
//                    numDirToMakeAtLvl[++lvlNum] = rand() % 4;
//                    
//                    // Find which level is the next to have a directory be created, specify that level to navigate to for next loop iteration
//                    if (numDirToMakeAtLvl[lvlNum] > 0)
//                    {
//                        moreDirToCreate = 1;
//                        currDirNumAtLvl[lvlNum] = 1;
//                        level = MIDDLE_LEVEL;
//                        
//                        // Save parent path for directory level 3
//                        strcpy(dirPathLvl2, newDirPath);
//                    }
//                    else if (numDirToMakeAtLvl[--lvlNum]> 0)
//                    {
//                        continue;
//                    }
//                }
//                
//                break;
        
            // Waldo Middle Level Directorys (number of level greater than 2, less than infinite)
            //case MIDDLE_LEVEL:
    
                strcpy(parentPath, dirPathLvl1);
                strcpy(grandParentPath, parentPath);
    
                while (numDirToMakeAtLvl[lvlNum] >= 0)
                {
                    // Create level 3 directory
                    sprintf(newDirPath, WALDO_DIR_NAME_FORMAT, parentPath, lvlNum, currDirNumAtLvl[lvlNum]++);
                    mkdir(newDirPath, 0700);
                    
                    // Log directory creation path
                    log_creation_path(creationLogFile, newDirPath);
                    
                    numDirToMakeAtLvl[lvlNum]--;
                    
                    // Get random num 1-3 and create that number of empty text files in new directory
                    create_rand3_file_num(newFilePath, newDirPath, creationLogFile, loremIpsumFilePath, WALDO_FILE_NAME_FORMAT);
                    
                    // Check next level won't exceed max
                    if ((lvlNum + 1) > LEVEL_MAX)
                    {
                        // Check if current level has more siblings
                        if (numDirToMakeAtLvl[lvlNum] > 0)
                        {
                            continue;
                        }
                        // Else go back to parent level
                        else
                        {
                            strcpy(parentPath, grandParentPath);
                            lvlNum--;
                        }
                    }
                    // Check next level is not first or last level
                    else if ((lvlNum + 1) > FIRST_CHILD_LVL && (lvlNum + 1) <= LEVEL_MAX)
                    {
                        // Get how many children direct children directories are to be created
                        numDirToMakeAtLvl[++lvlNum] = rand() % 4;
                        currDirNumAtLvl[lvlNum] = 1;

                        // Check if there are children directories to be made
                        if (numDirToMakeAtLvl[lvlNum] > 0)
                        {
                            // Save parent directory path
                            strcpy(parentPath, newDirPath);
                            strcpy(grandParentPath, parentPath);
                        }
                        // Else stay to iterate through siblings on current level
                        else
                        {
                            lvlNum++;
                        }
                    }
                    // Check if more siblings to create
                    else if (numDirToMakeAtLvl[lvlNum] > 0)
                    {
                        continue;
                    }
                    // Else traverse back up to parent directory
                    else
                    {
                        strcpy(parentPath, grandParentPath);
                        lvlNum--;
                    }
                }
                
                //break;
                
            // Waldo Level 4 Directories
//            case 4:
//                
//                dirNumLvl4 = 1;
//                
//                // Since it is the last directory level, loop and create directories until complete
//                while (mkDirLvl4 > 0)
//                {
//                    sprintf(newDirPath, WALDO_DIR_NAME_FORMAT, parentPath, lvlNum, dirNumLvl4++);
//                    mkdir(newDirPath, 0700);
//                    
//                    // Log directory creation path
//                    log_creation_path(creationLogFile, newDirPath);
//                    
//                    mkDirLvl4--;
//                    
//                    // Get random num 1-3 and create that number of empty text files in new directory
//                    create_rand3_file_num(newFilePath, newDirPath, creationLogFile, loremIpsumFilePath, WALDO_FILE_NAME_FORMAT);
//                }
//                
//                
//                // Check if the parent directory has any siblings that need to be created
//                if (mkDirLvl3 > 0)
//                {
//                    lvlNum = 3;
//                    strcpy(parentPath, dirPathLvl2);
//                }
//                // Parent has no siblings left to be create recurse back to grandparent
//                else
//                {
//                    lvlNum = 2;
//                }
//                
//                break;
        //}
    //}
    
    // Free memory of all pointers used for random asymmetrical directory tree creation
    free(WALDO_DIR_NAME_FORMAT);
    free(WALDO_FILE_NAME_FORMAT);
    free(parentDirName);
    free(parentPath);
    free(newDirPath);
    free(newFilePath);
    free(dirPathLvl2);
    
    // Close creation log file, free point
    if (creationLogFile != NULL)
    {
        fclose(creationLogFile);
    }
}



