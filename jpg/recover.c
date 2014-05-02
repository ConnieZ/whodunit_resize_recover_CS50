/**
 * recover.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Recovers JPEGs from a forensic image.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

typedef uint8_t BYTE;
#define BLOCK 512


int main(void)
{
    //load the file
    FILE* file;
    FILE* image = NULL;
    if ((file = fopen("card.raw", "r"))== NULL)
    {
        printf("Could not open 'card.raw'.\n");
        return 2;
    }
    
    int total = 0;
    int curr_writing = 0;
    char title[10];
    BYTE buffer[BLOCK] = {};
    
    while (fread(buffer, sizeof(BYTE), 512, file) == 512)
    {
                 
        //check if the first four bytes are a start of new file
        if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] == 0xe0 || buffer[3] == 0xe1))
        {
            //signal to start writing a new image
            curr_writing = 0;
        }
        else
        {
            curr_writing = 1;
        }
        
        //did we find the beginning of a new file? - yes!
        if (curr_writing == 0)
        {
            if(image)
            {
                //if we have a previous file open, close it
                fclose(image);
            }
            //start of new file
            sprintf(title, "%03d.jpg", total);
            image = fopen(title, "w");
            curr_writing = 1;
            
            //keep track
            total++;
        }
        //Is this start of a new file? - No!
        if (curr_writing == 1)
        {
            //are we already writing to a file?
            //yes!
            if(image)
            {
               fwrite(buffer, sizeof(BYTE), 512, image);
            }
        }
              
     }
     fclose(image);
     fclose(file);
     return 0;
}
