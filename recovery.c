/**
 * recover.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Recovers JPEGs from a forensic image.
 */


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct
{
    uint8_t ID_1;
    uint8_t ID_2;
    uint8_t ID_3;
    uint8_t ID_4;
    uint8_t body[508];
} block;

//declare functions and pointer to use later
int search(FILE* inptr);
void read_write (int location_start, FILE* inptr);
int JPEGs_found;

int main(int argc, char* argv[])
{    
    // open mem card file
    FILE* inptr = fopen("card.raw", "r");
    
    // check proper usage 
    if (inptr == NULL)
    {
        printf("Could not open file.\n");
        return 1;
    };    
      
    // search for placement of first jpeg block
    int location_start = search(inptr); 
    
    if (JPEGs_found == 1)
    {
        printf("No JPEGs found.");   
        return 1;   
    }
    else
    {
        printf("JPEGs found! Beginning recovery.");
    }
    
    // copy JPEGs starting at first jpeg block    
    read_write (location_start, inptr);
      
    //close input file
    fclose(inptr);
};


// searches for the location of the first jpeg block
int search(FILE* inptr)
{
    //declare a block to search
    block block_search;
    
    // declare an int to keep track of location
    int location_search;
    
    // declare an int to check if no JPEGs in file
    JPEGs_found = 1;
    
    // iterate over each block of infile and break when JPEG magic numbers are found
    for (location_search = 0; fread(&block_search, sizeof(block), 1, inptr) == 1; location_search++)
    {                            
        if (block_search.ID_1 == 0xff && block_search.ID_2 == 0xd8 && block_search.ID_3 == 0xff && block_search.ID_4 == 0xe0)
        {
            JPEGs_found = 0;
            break;
        }     
        else if (block_search.ID_1 == 0xff && block_search.ID_2 == 0xd8 && block_search.ID_3 == 0xff && block_search.ID_4 == 0xe1)
        {
            JPEGs_found = 0;
            break;
        };
    };
    
    // if JPEGs_found switch is 0, return the location of block containing first JPEG magic numbers
    if (JPEGs_found == 0)
    {
        return location_search;
    }
    // if JPEGs_found switch is 1, return the location of EOF   
    else
    {
        return location_search;
    };
};

void read_write (int location_start, FILE* inptr)
{
    // temporary buffer
    block buffer;
    
    // creates initial output file
    int JPEG_int = 0;
    char JPEG_str[8];
    sprintf(JPEG_str, "%03.3i.jpg", JPEG_int);    
    FILE* outptr = fopen(JPEG_str, "w");
    
    // copy location_start block
    fseek(inptr, sizeof(block) * location_start, SEEK_SET);
    fread(&buffer, sizeof(block), 1, inptr);
    fwrite(&buffer, sizeof(block), 1, outptr);
    
    while (fread(&buffer, sizeof(block), 1, inptr) == 1)
    { 
    
        if ((buffer.ID_1 == 0xff && buffer.ID_2 == 0xd8 && buffer.ID_3 == 0xff && buffer.ID_4 == 0xe0)
            ||
            (buffer.ID_1 == 0xff && buffer.ID_2 == 0xd8 && buffer.ID_3 == 0xff && buffer.ID_4 == 0xe1))
        {           
            fclose(outptr);
            JPEG_int++;
            sprintf(JPEG_str, "%03.3i.jpg", JPEG_int);
            FILE* outptr = fopen(JPEG_str, "w");
        };
        fwrite(&buffer, sizeof(block),1,outptr);
        //free(buffer);
    };
    
    fclose(outptr);
};
