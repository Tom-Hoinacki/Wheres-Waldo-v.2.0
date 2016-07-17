//
//  main.c
//  Where's Waldo?
//
//  Created by Thomas Hoinacki on 6/20/16.
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
#include "level1.h"
#include "tree_creator.h"
#include "tree_creator_breadth_first.h"
#include "sightings_log.h"




// MAIN
int main(int argc, const char * argv[])
{
    
    // Main Local Variables
    char cwd[1024];// cwd: current working directory
    char * dirPathLvl1;
    int pathLen;
    char *loremIpsumFilePath; // text filler file path
    char * sightingsLogPath;
    
    
    
    
    /* Get directory working directory path where this code is being executed
     from to act as starting point for creating directory tree */
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        
        
        /* CREATE LEVEL ONE DIRECTORY AND FILES */
        /****************************************/
        
        // Create Where's Waldo Level 1 top directory wherever code is run from on local machine
        get_dir_lvl1_Path(cwd, &dirPathLvl1, &pathLen);
        
        // Remove top level directory, files, children directories, and children files if exists so we can run this program repeatedly
        check_to_remove_existing_waldo_directory(dirPathLvl1);
        
        // Create top level directory
        mkdir(dirPathLvl1, 0700);
        
        // Create log file of Waldo sightings in top level directory
        create_sightings_log_file(dirPathLvl1, &sightingsLogPath, pathLen);
        
        // Create Lorem Ispum file inside top directory to copy text from for creating other text files
        create_text_filler_file(dirPathLvl1, &loremIpsumFilePath, pathLen);
        
        
        
        /* CREATE RANDOM ASYMMETRICAL CHILD DIRECTORY TREE
         * RANDOM BETWEEN 2-4 LEVELS OF CHILD DEPTH
         * RANDOM BETWEEN 1-3 CHILD DIRECTORIES CREATED WITHIN EACH DIRECTORY
         * RANDOM BETWEEN 1-3 TEXT FILES IN EACH DIRECTORY CREATED
         * RANDOM 1/100 CHANCE STRING "Waldo" IS INSERTED AFTER EACH WORD STREAMED AND PRINTED INTO EACH TEXT FILE
         /*********************************************************************************************************/
        
        //create_random_asym_dir_tree(dirPathLvl1, loremIpsumFilePath);
        create_breadth_first_random_asym_dir_tree(dirPathLvl1, loremIpsumFilePath);

        
        
        
        /* LOG ALL WALDO SIGHTINGS BY TRAVERSING ASYMMETRIC TREE DEPTH FIRST, STRING SEARCH EACH TEXT FILE FOR "Waldo" OCCURENCES */
        /**************************************************************************************************************************/
        
//        // Create log file, make writable
//        FILE * sightingsLogFile = fopen(sightingsLogPath, "w");
//        
//        if (sightingsLogFile == NULL)
//        {
//            fprintf(stderr, "Error: Failed to open sightingsLogFile - %s\n", strerror(errno));
//            return 1;
//        }
//        
//        // Traverse tree depth-first string searching one text file at a time and logging sightings
//        int sightingsCount = 1;
//        log_waldo_sightings_dir(dirPathLvl1, sightingsLogFile, &sightingsCount);
//        fclose(sightingsLogFile);
    }
    else
    {
        perror("getcwd() error");
    }
    
    return 0;
}


