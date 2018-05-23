/**
 * romsplit - a tool for splitting binary ROM images into two or more chunks.
 *
 *  Written in an attempt to extract UEFI and EC images 
 *   from firmware update ROMs so that the flash chips 
 *   can be written to with the aid of flashrom or similar
 *   utilities.
 *
 */
#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <string.h>

unsigned long *chunkSizes;
char *fileBuffer;
long fileSize = 0;

int main(int argc, char* argv[])
{
	//Determine if we have the minimum number of arguments and display
	// usage information if not.	
	if(argc <= 3)
	{
		printf("romsplit - split binary files into two or more seperate chunks\n");
		printf("Usage: %s <filename> <size of chunk 1> <size of chunk 2> ... <size of chunk n>\n\n", argv[0]);
		printf("You must specify a filename and at least two chunk sizes\n"); 
		exit(0);
	}

	//Allocate space for n integers, depending on how many
	// chunk sizes are specified on the command line.  We
	// do argc - 2 because args[0] and args[1] will be the
	// name of the program and the filename of the image to
	// split, respectively.
	chunkSizes = calloc(argc - 2, sizeof(unsigned long));	
	
	unsigned long total = 0;

	for(int i = 0; i < argc; i++)
	{
		printf("arg[%d]: %s\n", i, argv[i]);

		if(i >= 2)
		{
			int lastCharacter = strlen(argv[i]) - 1;
			char c = argv[i][lastCharacter];
			long multiplier = 1;

			switch(c)
			{
				case 'k':
				case 'K':
					//Working in kilobytes - multiply by 1024
					printf("K multiplier specified\n");
					multiplier = 1024;
					break;
				case 'm':
				case 'M':
					//Working in megabytes - multiply by 1048576
					printf("M multiplier specified\n");
					multiplier = 1024 * 1024;
					break;
				case 'b':
				case 'B':
					printf("Working with bytes\n");
					break;
				default:
					//Multiplier character not specified so
					// increase the character count
					// so that we get the entire number
					printf("Working with bytes\n");
					lastCharacter++;
			}

			//Now parse the integer contained in the input string and
			//  multiply it by the unit size we are working with
			char *substring = calloc(sizeof(char), lastCharacter);
			strncpy(substring, argv[i], lastCharacter);
			chunkSizes[i - 2] = atol(substring) * multiplier;
			total += chunkSizes[i - 2];
			printf("size: %d\n", chunkSizes[i - 2]);
			free(substring);
		}
	}

	//Attempt to open the file specified in the first argument
	// to the program (open as read-only binary);
	FILE *f = fopen(argv[1], "rb");
	if(f == NULL)
	{
		printf("ERROR: Can't open file %s (error %d)\n", argv[1], errno);
		exit(1);
	}
	else
	{
		printf("Opening file\n");
	}

	fseek(f, 0, SEEK_END);
	fileSize = ftell(f);
	if(fileSize == -1)
	{
		printf("ERROR: Can't determine file size (error %d)\n", errno);
	}
	else
	{
		printf("Input file size: %d\n", fileSize);

		if(total != fileSize)
		{
			printf("ERROR:  Total chunk sizes must match the input file size.\n");
			exit(1);
		}

		fileBuffer = calloc(sizeof(char), fileSize);
		rewind(f);
		int bytesRead = fread(fileBuffer, sizeof(char), fileSize, f);
		printf("bytes read: %d\n", bytesRead);
	}

	if(fclose(f) == EOF)
	{
		printf("ERROR: Problemn closing file (error %d)\n", errno);
	}
	else
	{
		printf("Closing file\n");
	}
	
	//Create a new output filename by appending the chunk number
	// to the end of the input filename, and write out the specified
	// number of bytes from the input file buffer, starting at the
	// appropriate boundaries in the input buffer
	unsigned long memptr = 0;

	for(int i = 0; i < argc - 2; i++)
	{
		char extension[3];
		memset(extension, 0, sizeof(extension));
		sprintf(extension, ".%d", i);
		char *newFilename = calloc(sizeof(char), strlen(argv[1]) + sizeof(extension));	
		memcpy(newFilename, argv[1], strlen(argv[1]));
		strcat(newFilename, extension);
		printf("Writing %s at offset %d... ",  newFilename, memptr);

		FILE *newFile = fopen(newFilename, "wb");

		if(newFile == NULL)
		{
			printf("ERROR: Can't write new file %s (error %d)\n", newFilename, errno);
			exit(1);
		}

		fwrite(&fileBuffer[memptr], chunkSizes[i], 1, newFile);
		fclose(newFile);
		printf("done\n");
		memptr += chunkSizes[i];
		free(newFilename);
	}

	free(fileBuffer);
	free(chunkSizes);

    return 0;
}




