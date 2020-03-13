// TagForceMusicEx.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdlib.h>
#include <string.h>

void* EntryBuffer;
unsigned int AT3FileSize;
char OutFileName[512];
char OutPath[512];
char MkDirString[_MAX_PATH + 10];

struct snddat_entry
{
	short int StartPointerMagic; // always SP or 0x5053
	short int Index;
	unsigned int EntrySize;
	unsigned int InfoHeaderSize;
	unsigned int unk2;
	unsigned int unk3;
	unsigned int FullHeaderSize;
	unsigned int unk5;
	unsigned int unk6;
}Entry;

int ExtractSndDat(const char* InFileName, const char* OutFilePath)
{	
	FILE* InFile = fopen(InFileName, "rb");
	FILE* OutFile;
	unsigned int counter = 0;


	if (InFile == NULL)
	{
		printf("ERROR: Error opening file for reading: %s\n", InFileName);
		perror("ERROR");
		return -1;
	}

	while (!feof(InFile))
	{
		// Read entry info
		fread(&Entry, sizeof(snddat_entry), 1, InFile);
		fseek(InFile, -(int)(sizeof(snddat_entry)), SEEK_CUR);
		EntryBuffer = malloc(Entry.EntrySize);
		// Load entire entry into memory
		fread(EntryBuffer, Entry.EntrySize, 1, InFile);
		// Read AT3 size
		AT3FileSize = *(unsigned int*)(((unsigned int)EntryBuffer) + Entry.FullHeaderSize);
		// Generate filename & open file
		sprintf(OutFileName, "%s\\%d.at3", OutFilePath, counter);
		printf("EXTRACTING: %s\n", OutFileName);
		OutFile = fopen(OutFileName, "wb");
		if (OutFile == NULL)
		{
			printf("ERROR: Error opening file for writing: %s\n", OutFilePath);
			perror("ERROR");
			return -1;
		}
		fwrite((void*)(((unsigned int)EntryBuffer) + Entry.FullHeaderSize + 4), AT3FileSize, 1, OutFile);
		// cleanup & increase counter
		fclose(OutFile);
		free(EntryBuffer);
		counter++;
	}
	fclose(InFile);
	return 0;
}

int main(int argc, char *argv[])
{
	printf("Yu-Gi-Oh! Tag Force SNDDAT Extractor\n");
	if (argc < 2)
	{
		printf("ERROR: Too few arguments.\nUSAGE: %s psp_snddat.bin [OutDir]\n", argv[0]);
		return -1;
	}

	if (argv[2] != NULL)
		strcpy(OutPath, argv[2]);
	else
	{
		char* autogen;
		strcpy(OutPath, argv[1]);
		autogen = strrchr(OutPath, '.');
		if (autogen)
			*autogen = 0;
	}

	printf("Creating directory: %s\n", OutPath);
	sprintf(MkDirString, "mkdir \"%s\"", OutPath);
	system(MkDirString);

    return ExtractSndDat(argv[1], OutPath);
}

