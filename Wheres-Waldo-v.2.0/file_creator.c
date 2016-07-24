//
//  file_creator.c
//  Wheres-Waldo-v.2.0
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
#include "file_creator.h"
#include "level1.h"


// Function Signatures
void create_rand3_file_num(char * newFilePath, char * newDirPath, FILE * creationLogFile, char * loremPath, char * const format);



// Function Implementations

/* Creates between 1-3 (random) files for every child directory created in the Waldo tree */
void create_rand3_file_num(char * newFilePath, char * newDirPath, FILE * creationLogFile, char * loremPath, char * const format)
{
    int filesToCreate = rand() % 3 + 1;
    int fileNum = 1;
    
    while (filesToCreate > 0)
    {
        sprintf(newFilePath, format, newDirPath, fileNum++);
        
        size_t n = 0;
        int c;
        
        FILE* fd = fopen(newFilePath, "ab+");
        
        if (fd != NULL)
        {
            // Log directory creation path
            log_creation_path(creationLogFile, newFilePath);
            
            // Write random Lorem Ipsum to file with random Waldos
            FILE* fdLorem = fopen(loremPath, "r");
            
            int lineNum = 1;
            int chCount = 0;
            const int  MAX_CHAR_LAST_WORD = 75;
            
            
            // Process Lorem Ipsum text file one char at a time to print to current Waldo Level text file
            while ((c = fgetc(fdLorem)) != EOF)
            {
                if ((char) c == ' ')
                {
                    // 1 in 100 chance Waldo will be printed after each word
                    int printWaldo = rand() % 100 + 1;
                    if (printWaldo == 100)
                    {
                        fprintf(fd, " Waldo");
                        chCount += 6;
                    }
                }
                
                // If start of line print line number and tab
                if (chCount == 0)
                {
                    fprintf(fd, "%d", lineNum);
                    lineNum++;
                    fputs("\t", fd);
                    
                    // Print character, increase count
                    fputc((char) c , fd);
                    chCount++;
                }
                // Else if the character counter has reached the maximum for the last word check for whitespace
                // to start a new line and reset the character counter
                else if (chCount >= MAX_CHAR_LAST_WORD)
                {
                    if ((char) c == ' ')
                    {
                        fputs("\n", fd);
                        chCount = 0;
                    }
                }
                else
                {
                    // Print character, increase count
                    fputc((char) c , fd);
                    chCount++;
                }
            }
            
            filesToCreate--;
            fclose(fd);
        }
    }
}
