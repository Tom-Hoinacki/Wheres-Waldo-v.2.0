//
//  sightings_log.h
//  Waldo v1.2
//
//  Created by Thomas Hoinacki on 7/16/16.
//  Copyright (c) 2016 Thomas Hoinacki. All rights reserved.
//

#ifndef __Waldo_v1_2__sightings_log__
#define __Waldo_v1_2__sightings_log__

#include <stdio.h>

void log_waldo_sightings_dir_depth_first(char * dirPath, FILE * sightingsLogFile, int * sightingsCount, int * firstLevel);

void log_waldo_sightings_txtfile(char * dirPath, struct dirent * in_Dir, FILE * sightingsLogFile, int * sightingsCount);

#endif