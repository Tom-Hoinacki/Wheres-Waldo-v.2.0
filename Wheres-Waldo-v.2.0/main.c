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
#include "tree_creator_breadth_first.h"
#include "tree_creator_depth_first.h"
#include "sightings_log.h"



// MAIN
int main(int argc, const char * argv[])
{
    
    // Main Local Variables
    char cwd[1024];// cwd: current working directory
    char * dirPathLvl1_Breadth, * dirPathLvl1_Depth;
    int pathLen;
    char * loremIpsumFilePath; // text filler file path
    
    // Directory creation log paths
    char * createBreadthLogPath = (char *)malloc(PATH_MAX);
    char * createDepthLogPath = (char *)malloc(PATH_MAX);
    char * createLogNameBreadth = (char *)malloc(strlen(1 + "/Breadth-First Directory Creation Log.txt"));
    strcpy(createLogNameBreadth, "/Breadth-First Directory Creation Log.txt");
    char * createLogNameDepth = (char *)malloc(strlen(1 + "/Depth-First Directory Creation Log.txt"));
    strcpy(createLogNameDepth, "/Depth-First Directory Creation Log.txt");

    // Waldo sightings log paths
    char * sightingsBreadthLogPath = (char *)malloc(PATH_MAX);
    char * sightingsDepthLogPath = (char *)malloc(PATH_MAX);
    char * sightingsLogNameBreadth = (char *)malloc(strlen("/Breadth-First Sightings Log.txt") + 100);
    strcpy(sightingsLogNameBreadth, "/Breadth-First Sightings Log.txt");
    char * sightingsLogNameDepth = (char *)malloc(strlen(1 + "/Depth-First Sightings Log.txt"));
    strcpy(sightingsLogNameDepth, "/Depth-First Sightings Log.txt");
    
    // Log sightings variables
    int sightingsDepthCount, sightingsBreadthCount;
    int * firstLevel = 1;
    
    // Output directory creation variables
    FILE * outputFd;
    int mkDirStatus;
    char * waldoOutputPath = (char *)malloc(PATH_MAX);
    strcpy(waldoOutputPath, "/Users/Tommy/Desktop/Wheres-Waldo-Output");
    
    // Remove same named directory and all its folders and files if already exists
    check_to_remove_existing_waldo_directory(waldoOutputPath);
    
    // Make output directory on desktop
    mkDirStatus = mkdir(waldoOutputPath, 0700);
    
    
    if (mkDirStatus == 0)
    {
        /*    CREATE OUTPUT & LEVEL 1 DIRECTORIES WITH TEXT FILLER AND LOG FILES    */
        /****************************************************************************/

        // Create Where's Waldo Level 1 directory paths for breadth and depth traversals
        get_dir_lvl1_Paths(waldoOutputPath, &dirPathLvl1_Breadth, &dirPathLvl1_Depth, &pathLen);

        // Create Level 1 directories
        //mkdir(dirPathLvl1_Breadth, 0700);
        mkdir(dirPathLvl1_Depth, 0700);

        // Create directory creation and sightings log files in Level 1 directories
        // CREATION LOGS
        create_log_file(dirPathLvl1_Breadth, &createBreadthLogPath, createLogNameBreadth);
        create_log_file(dirPathLvl1_Depth, &createDepthLogPath, createLogNameDepth);
        free(createLogNameBreadth);
        free(createLogNameDepth);

        // SIGHTINGS LOGS
        create_log_file(dirPathLvl1_Breadth, &sightingsBreadthLogPath, sightingsLogNameBreadth);
        create_log_file(dirPathLvl1_Depth, &sightingsDepthLogPath, sightingsLogNameDepth);
        free(sightingsLogNameBreadth);
        free(sightingsLogNameDepth);

        // Create Lorem Ispum file inside top directory to copy text from for creating other text files
        create_text_filler_file(waldoOutputPath, &loremIpsumFilePath, pathLen);



        /* CREATE RANDOM ASYMMETRICAL CHILD DIRECTORY TREE (BREADTH-FIRST & DEPTH FIRST SEPARATELY)
         * RANDOM BETWEEN 2-4 LEVELS OF DEPTH
         * RANDOM BETWEEN 1-3 CHILD DIRECTORIES CREATED WITHIN EACH DIRECTORY
         * RANDOM BETWEEN 1-3 TEXT FILES CREATED IN EACH DIRECTORY
         * RANDOM 1/100 CHANCE STRING "Waldo" IS PRINTED TO TEXT FILE AFTER EACH WORD READ FROM FILE STREAM
        /*********************************************************************************************************/

        // BREADTH-FIRST RANDOM ASYMMETRICAL DIRECTORY TREE CREATION
//        create_breadth_first_random_asym_dir_tree(dirPathLvl1_Breadth, createBreadthLogPath, loremIpsumFilePath);
//        free(createBreadthLogPath);

        // DEPTH-FIRST RANDOM ASYMMETRICAL DIRECTORY TREE CREATION
        create_depth_first_random_asym_dir_tree(dirPathLvl1_Depth, createDepthLogPath, loremIpsumFilePath);
        free(createDepthLogPath);



        /* LOG ALL WALDO SIGHTINGS BY TRAVERSING ASYMMETRIC TREE BREADTH & DEPTH FIRST  */
        /* STRING SEARCH EACH TEXT FILE FOR "Waldo" OCCURENCES                          */
        /********************************************************************************/

        // (BREADTH-FIRST) Open log file as writable
//        FILE * sightingsBreadthLogFile = fopen(sightingsBreadthLogPath, "w");
//
//        if (sightingsBreadthLogFile == NULL)
//        {
//            fprintf(stderr, "Error: Failed to open sightingsBreadthLogFile - %s\n", strerror(errno));
//            return 1;
//        }
//
//        // Traverse tree depth-first string searching one text file at a time and logging sightings
//        log_waldo_sightings_dir_breadth_first(dirPathLvl1_Breadth, sightingsBreadthLogFile, &sightingsBreadthCount);
//        fclose(sightingsBreadthLogFile);
//        free(sightingsBreadthLogPath);
        

        // (DEPTH-FIRST) Open sightings log file as writable
        FILE * sightingsDepthLogFile = fopen(sightingsDepthLogPath, "w");
        
        if (sightingsDepthLogFile == NULL)
        {
            fprintf(stderr, "Error: Failed to open sightingsDepthLogFile - %s\n", strerror(errno));
            return 1;
        }
        
        // Recursively traverse tree depth-first string searching one text file at a time and logging sightings
        log_waldo_sightings_dir_depth_first(dirPathLvl1_Depth, sightingsDepthLogFile, &sightingsDepthCount, &firstLevel);
        fclose(sightingsDepthLogFile);
        free(sightingsDepthLogPath);
    }
    else
    {
        fprintf(stderr, "Error: Failed to create output directory - %s\n", strerror(errno));
    }

    return 0;
}


