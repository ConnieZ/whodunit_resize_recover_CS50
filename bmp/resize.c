/**
 * resize.c
 *
 * by ConnieZ
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./copy n infile outfile\n");
        return 1;
    }
    
    // remember filenames and multiple
    int n = atoi (argv[1]);
    char* infile = argv[2];
    char* outfile = argv[3];
    
    if (n < 1)
    {
        printf("Enter a positive integer\n");
        return 2;
    }
    
    if (n > 100)
    {
        printf("Enter a positive integer under 100\n");
        return 3;
    }
    
    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE* outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    
    //keep track of the original width, height, and padding
    int orig_width = bi.biWidth;
    int orig_height = bi.biHeight;
    int orig_padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    //update header info to new width and height to then store in the output file
    bi.biWidth = bi.biWidth*n;
    bi.biHeight = bi.biHeight*n;
    
    // determine new padding for scanlines
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
      
    //update the Image and File Size
    bi.biSizeImage = (bi.biWidth*bi.biBitCount + 31)/32*4*abs(bi.biHeight);
    bf.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bi.biSizeImage;
       
    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    
    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(orig_height); i < biHeight; i++)
    {
        //tracker of beginning of row
        fpos_t beg_row;
        fgetpos(inptr, &beg_row);
        for (int j = 0; j < n; j++)
        {
            //checker to see if we need to go to beginning of row
            if (j < n)
            {
                fsetpos(inptr, &beg_row);
            }
            // iterate over pixels in scanline
            for (int k = 0; k < orig_width; k++)
            {
                // temporary storage
                RGBTRIPLE triple;

                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                // write RGB triple to outfile
                for (int m = 0; m < n; m++)
                {
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
            }

            // skip over padding, if any
            fseek(inptr, orig_padding, SEEK_CUR);

            // then add it back (to demonstrate how)
            for (int l = 0; l < padding; l++)
            {
                fputc(0x00, outptr);
            }
            
        }
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // close out
    return 0;
}
