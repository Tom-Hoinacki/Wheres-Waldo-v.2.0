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
    char * dirPathLvl1_Breadth, * dirPathLvl1_Depth;
    int pathLen;
    char * loremIpsumFilePath; // text filler file path
    
    
    char * createBreadthLogPath, * createDepthLogPath;
    
    char * createLogNameBreadth = (char *)malloc(strlen(1 + "/_Waldo Breadth-First Directory Creation Log.txt"));

    strcpy(createLogNameBreadth, "/_Waldo Breadth-First Directory Creation Log.txt");
    
    char * createLogNameDepth = (char *)malloc(strlen(1 + "/_Waldo Depth-First Directory Creation Log.txt"));
    strcpy(createLogNameDepth, "/_Waldo Depth-First Directory Creation Log.txt");

    
    char * sightingsBreadthLogPath, * sightingsDepthLogPath;
    char * sightingsLogNameBreadth = (char *)malloc(strlen(1 + "/_Waldo Breadth-First Sightings Log.txt"));
    strcpy(sightingsLogNameBreadth, "/_Waldo Breadth-First Sightings Log.txt");
    char * sightingsLogNameDepth = (char *)malloc(strlen(1 + "/_Waldo Depth-First Sightings Log.txt"));
    strcpy(sightingsLogNameDepth, "/_Waldo Depth-First Sightings Log.txt");
    
    
    /* Get directory working directory path where this code is being executed
     from to act as starting point for creating directory tree */
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        
        
        /* CREATE LEVEL ONE DIRECTORY AND FILES */
        /****************************************/
        
        // Create Where's Waldo Level 1 top directory wherever code is run from on local machine
        get_dir_lvl1_Paths(cwd, &dirPathLvl1_Breadth, &dirPathLvl1_Depth, &pathLen);
        
        // Remove top level directory, files, children directories, and children files if exists so we can run this program repeatedly
        check_to_remove_existing_waldo_directories(dirPathLvl1_Breadth, dirPathLvl1_Depth);
        
        // Create top level directories, breadth-first & depth-first
        mkdir(dirPathLvl1_Breadth, 0700);
        mkdir(dirPathLvl1_Depth, 0700);
        
        /* Create directory creation and sightings log files in top level directory */
        create_log_file(dirPathLvl1_Breadth, &createBreadthLogPath, createLogNameBreadth);
        create_log_file(dirPathLvl1_Depth, &createDepthLogPath, createLogNameDepth);
        
        create_log_file(dirPathLvl1_Breadth, &sightingsBreadthLogPath, sightingsLogNameBreadth);
        create_log_file(dirPathLvl1_Depth, &sightingsDepthLogPath, sightingsLogNameDepth);
        
        free(createLogNameBreadth);
        free(createLogNameDepth);
        free(sightingsLogNameBreadth);
        free(sightingsLogNameDepth);

        // Create Lorem Ispum file inside top directory to copy text from for creating other text files
        create_text_filler_file(dirPathLvl1_Breadth, &loremIpsumFilePath, pathLen);
        create_text_filler_file(dirPathLvl1_Depth, &loremIpsumFilePath, pathLen);
        
        /* CREATE RANDOM ASYMMETRICAL CHILD DIRECTORY TREE (BREADTH-FIRST & DEPTH FIRST SEPARATELY)
         * RANDOM BETWEEN 2-4 LEVELS OF CHILD DEPTH
         * RANDOM BETWEEN 1-3 CHILD DIRECTORIES CREATED WITHIN EACH DIRECTORY
         * RANDOM BETWEEN 1-3 TEXT FILES IN EACH DIRECTORY CREATED
         * RANDOM 1/100 CHANCE STRING "Waldo" IS INSERTED AFTER EACH WORD STREAMED AND PRINTED INTO EACH TEXT FILE
         /*********************************************************************************************************/
        
         // BREADTH-FIRST RANDOM ASYMMETRICAL DIRECTORY TREE CREATION
         create_breadth_first_random_asym_dir_tree(dirPathLvl1_Breadth, loremIpsumFilePath);
        
         // DEPTH-FIRST RANDOM ASYMMETRICAL DIRECTORY TREE CREATION
         create_depth_first_random_asym_dir_tree(dirPathLvl1_Depth, loremIpsumFilePath);

        
        
        /* LOG ALL WALDO SIGHTINGS BY TRAVERSING ASYMMETRIC TREE BREADTH & DEPTH FIRST, STRING SEARCH EACH TEXT FILE FOR "Waldo" OCCURENCES */
        /************************************************************************************************************************************/
        
        //Create log file, make writable (BREADTH-FIRST)
        FILE * sightingsBreadthLogFile = fopen(sightingsBreadthLogPath, "w");
        
        if (sightingsBreadthLogFile == NULL)
        {
            fprintf(stderr, "Error: Failed to open sightingsBreadthLogFile - %s\n", strerror(errno));
            return 1;
        }
        
        // Traverse tree depth-first string searching one text file at a time and logging sightings
        int sightingsBreadthCount = 1;
        log_waldo_sightings_dir_breadth_first(dirPathLvl1_Breadth, sightingsBreadthLogFile, &sightingsBreadthCount);
        fclose(sightingsBreadthLogFile);
        
        
        // Create log file, make writable (DEPTH-FIRST)
        FILE * sightingsDepthLogFile = fopen(sightingsDepthLogPath, "w");
        
        if (sightingsDepthLogFile == NULL)
        {
            fprintf(stderr, "Error: Failed to open sightingsDepthLogFile - %s\n", strerror(errno));
            return 1;
        }
        
        // Recursively traverse tree depth-first string searching one text file at a time and logging sightings
        int sightingsDepthCount = 1;
        log_waldo_sightings_dir_depth_first(dirPathLvl1_Depth, sightingsDepthLogFile, &sightingsDepthCount);
        fclose(sightingsDepthLogFile);

        
    }
    else
    {
        perror("getcwd() error");
    }
    
    return 0;
}


