//
//  level1.h
//  Waldo v1.2
//
//  Created by Thomas Hoinacki on 7/14/16.
//  Copyright (c) 2016 Thomas Hoinacki. All rights reserved.
//

#ifndef __Waldo_v1_2__level1__
#define __Waldo_v1_2__level1__

#include <stdio.h>

void get_dir_lvl1_Paths(char * currentWorkingDirectory, char ** dirPathLvl1_Breadth, char ** dirPathLvl1_Depth, int * pathLen);

void check_to_remove_existing_waldo_directory(char * outputPath);

void create_log_file(char * dirPathLvl1, char ** sightingsLogPath, char * logName);

void create_text_filler_file(char * waldoOutputPath, char ** loremIpsumFilePath, int pathLen);

void log_creation_path(FILE * logPath, char * newPath);

#endif

