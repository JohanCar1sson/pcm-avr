/* wav2h: converts mono (single channel), 8 kHz sample rate WAV file into a
 * data array in a header file that can be used by my pcm-avr library.
 *
 * SoX can be used to produce the input audio file, for example:
 * sox Growling.mp3 -c 1 -r 8k -b 8 -e unsigned-integer dog.wav speed 0.9 norm
 *
 * Copyright 2022 Johan Carlsson
 */

/* wave2c, a WAV file to GBA C source converter.
 * Copyright (c) 2003 by Mathieu Brethes.
 *
 * Contact : thieumsweb@free.fr
 * Website : http://thieumsweb.free.fr/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdlib.h>
#include <alloca.h>
#include <string.h>
#include <stdio.h>

typedef struct
{
	int sampleRate;
	int numChannels;
	int bitDepth;
	int dataLength;
} wavSound;

/* Loads a wave header in memory, and checks for its validity. */
/* returns NULL on error, a malloced() wavSound* otherwise.    */
wavSound * loadWaveHeader(FILE * fp)
{
	char c[5];
	int nbRead;
	int chunkSize;
	int subChunk1Size;
	int subChunk2Size;
	short int audFormat;
	short int nbChannels;
	int sampleRate;
	int byteRate;
	short int blockAlign;
	short int bitDepth;
	wavSound *w;

	c[4] = 0;

	nbRead=fread(c, sizeof(char), 4, fp);
	
	/* EOF ? */
	if (nbRead < 4) return NULL;
	
	/* Not a RIFF ? */
	if (strcmp(c, "RIFF") != 0)
	{
		printf("Not a RIFF: %s\n", c);
		return NULL;
	}
	nbRead=fread(&chunkSize, sizeof(int), 1, fp);
	
	/* EOF ? */
	if (nbRead < 1) return NULL;

	nbRead=fread(c, sizeof(char), 4, fp);
	
	/* EOF ? */
	if (nbRead < 4) return NULL;
	
	/* Not a WAVE riff ? */
	if (strcmp(c, "WAVE") != 0)
	{
		printf("Not a WAVE: %s\n", c);
		return NULL;
	}
	nbRead=fread(c, sizeof(char), 4, fp);
	
	/* EOF ? */
	if (nbRead < 4) return NULL;
	
	/* Not a "fmt " subchunk ? */
	if (strcmp(c, "fmt ") != 0)
	{
		printf("No fmt subchunk: %s\n", c);
		return NULL;
	}
	/* read size of chunk. */
	nbRead=fread(&subChunk1Size, sizeof(int), 1, fp);
	if (nbRead < 1) return NULL;

	/* is it a PCM ? */
	if (subChunk1Size != 16)
	{
		printf("Not PCM fmt chunk size: %d\n", subChunk1Size);
		return NULL;
	}
	nbRead=fread(&audFormat, sizeof(short int), 1, fp);
	if (nbRead < 1) return NULL;

	/* is it PCM ? */
	if (audFormat != 1)
	{
		printf("No PCM format (1): %dh\n", audFormat);
		return NULL;
	}
	nbRead=fread(&nbChannels, sizeof(short int), 1, fp);
	if (nbRead < 1) return NULL;

	/* is it mono ? */
	if (nbChannels != 1)
	{
		printf("Number of channels invalid: %dh (must be mono)\n", nbChannels);
		return NULL;
	}
	nbRead=fread(&sampleRate, sizeof(int), 1, fp);
	if (nbRead < 1) return NULL;

	/* is the sample rate 8 kHz ? */
	if (sampleRate != 8000)
	{
		printf("The sample rate is invalid: %d (must be 8 kHz)\n", sampleRate);
		return NULL;
	}
	nbRead=fread(&byteRate, sizeof(int), 1, fp);
	if (nbRead < 1) return NULL;

	nbRead=fread(&blockAlign, sizeof(short int), 1, fp);
	if (nbRead < 1) return NULL;

	nbRead=fread(&bitDepth, sizeof(short int), 1, fp);
	if (nbRead < 1) return NULL;

	/* is the bit depth 8 bits? */
	if (bitDepth != 8)
	{
		printf("The bit depth is invalid: %dh (must be 8 bits)\n", bitDepth);
		return NULL;
	}
	nbRead=fread(c, sizeof(char), 4, fp);
	
	/* EOF ? */
	if (nbRead < 4) return NULL;
	
	/* Not a data section ? */
	if (strcmp(c, "data") != 0)
	{
		printf("Not a data subchunk: %s\n", c);
		return NULL;
	}
	nbRead=fread(&subChunk2Size, sizeof(int), 1, fp);
	if (nbRead < 1) return NULL;

	/* Now we can generate the structure... */

	w = (wavSound *) malloc(sizeof(wavSound));
	/* out of memory ? */
	if (w == NULL)
	{
		printf("Out of memory, sorry\n");	
		return w;
	}
	w->sampleRate = 8000;
	w->numChannels = 1;
	w->bitDepth = 8;
	w->dataLength = subChunk2Size;

	return w;
}

/* Loads the actual wave data into the data structure */
void saveWave(FILE *fpI, wavSound *s, FILE *fpO, char *name)
{
	long filepos;
	int i, j;
	int realLength, numchars;
	unsigned char stuff8, databyte, crushfactor = 8 / s->bitDepth, nb, nb8;

	/* nb = number of bits used for each audio sample */
	nb = 8 / crushfactor; nb8 = 8 - nb;

	filepos = ftell(fpI);
	
	/* Print general information */
	fprintf(fpO, "/* %s sound made by wav2h */\n\n", name);
	fprintf(fpO, "#ifndef RAUDIO_PREFIX\n#define RAUDIO_PREFIX\n#endif\n\n");
	fprintf(fpO, "#define KONK(a, b) KONK_(a, b)\n#define KONK_(a, b) a ## b\n\n");
	fprintf(fpO, "const unsigned char KONK(RAUDIO_PREFIX, raudio_bitdepth) = %d;\n", s->bitDepth);

	/* realLength = (s->dataLength / s->numChannels / s->bitDepth * 8); */
	realLength = s->dataLength;
	numchars = (realLength + crushfactor - 1) / crushfactor; /* Round up */
	/* printf("realLength = %d, numchars = %d, crushfactor = %d\n", realLength, numchars, crushfactor); */

	fprintf(fpO, "const unsigned int KONK(RAUDIO_PREFIX, raudio_length) = %d;\n\n", numchars /* * crushfactor */);
	fprintf(fpO, "const unsigned char KONK(RAUDIO_PREFIX, raudio_data)[] PROGMEM = {\n");

	for (i = 0; i < numchars; i++)
	{
		databyte = 0;
		for (j = 0; j < crushfactor; j++)
		{
			stuff8 = 128; /* a.k.a. silence */
			if (i * crushfactor + j < realLength)
				fread(&stuff8, sizeof(unsigned char), 1, fpI);
			/* stuff8 = i * crushfactor + j;
			printf("%d, %d, %hu\n", i, j, stuff8); */
			/* Do bit crushing here */
			databyte >>= nb;
			databyte += (stuff8 >> nb8) << nb8;
		}
		/* databyte = 255; */
		fprintf(fpO, "%3d%s", databyte, (i < numchars - 1) ? ", " : "");
		if (i < numchars - 1 && (i + 1) % 16 == 0) fprintf(fpO, "\n");
	}
	fprintf(fpO, "};\n\n#undef RAUDIO_PREFIX\n");
}

int main(int argc, char *argv[])
{
	wavSound *s;
	FILE *fin;
	FILE *fout;
	int namelen, idot,bd;
	char *name;

	if (argc != 3)
	{
		printf("Usage: %s <file.wav> <bit depth>\n", argv[0]);
		exit(0);
	}
    namelen = strlen(argv[1]);
    idot = namelen - 4;
    name = (char *)alloca(namelen);
    strcpy(name, argv[1]);
    /* printf("idot = %d, ->%s<- ->%s<-\n", idot, name, name + idot); exit(0); */
    if (strcmp(name + idot, ".wav"))
    {
		printf("The input file %s does not have the required \".wav\" suffix\n", argv[1]);
		exit(0);
    }
    name[idot] = '\0';
    /* printf("->%s<-\n", name); exit(0); */
	if (!(fin = fopen(argv[1], "r")))
	{
		printf("The input file %s cannot be opened\n", argv[1]);
		exit(0);
	}
	if ((s = loadWaveHeader(fin)) == NULL)
	{
		printf("The input file %s does not have the correct format\n", argv[1]);
		exit(0);
	}
	bd = atoi(argv[2]);
	if (!(bd == 8 || bd == 4 || bd == 2 || bd == 1))
	{
		printf("%s is not an acceptable bit depth (must be 8, 4, 2 or 1)\n", argv[2]);
		exit(0);
	}
	s->bitDepth = bd;

    strcat(name, ".h");
    /* printf("->%s<-\n", name); exit(0); */
	fout = fopen(name, "w");
	name[idot] = '\0';

	saveWave(fin, s, fout, name);
	fclose(fin);
	fclose(fout);

	return 0;	
}
