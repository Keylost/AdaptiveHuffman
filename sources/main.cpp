#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#include "huffman.hpp"

void usage();

int main(int argc, char **argv)
{
	char *filePathEnc = NULL; //path to file
	char *filePathDec = NULL; //path to file
	char *filePathOut = NULL; //path to file
	
	const char* short_options = "he:d:o:";
	const struct option long_options[] =
	{
		{"help",no_argument,NULL,'e'},
		{"encode",required_argument,NULL,'e'},
		{"decode",required_argument,NULL,'d'},
		{"output",required_argument,NULL,'o'},
		{NULL,0,NULL,0}
	};
	
	int rez;
	int option_index;

	while ((rez=getopt_long(argc,argv,short_options,long_options,&option_index))!=-1)
	{
			switch(rez)
			{
				case 'h': 
				{
					usage();
					break;
				};
				case 'e':
				{
					if(filePathDec != NULL)
					{
						fprintf(stderr,"[W]: Only -e or -d can be specified at once\n");
						break;
					}
					filePathEnc = new char[strlen(optarg)+1];
					strcpy(filePathEnc,optarg);
					
					break;
				};				
				case 'd':
				{
					if(filePathEnc != NULL)
					{
						fprintf(stderr,"[W]: Only -e or -d can be specified at once\n");
						break;
					}					
					filePathDec = new char[strlen(optarg)+1];
					strcpy(filePathDec,optarg);
					break;
				};
				case 'o':
				{				
					filePathOut = new char[strlen(optarg)+1];
					strcpy(filePathOut,optarg);
					break;
				};
				default:
				{
					fprintf(stderr,"[E]: Found unknown option\n");
					usage();
					break;
				};
			};
	};
	
	if(filePathOut == NULL)
	{
		fprintf(stderr,"[E]: -o option must be specified\n");
	}
	
	if(filePathEnc != NULL || filePathDec != NULL)
	{
		if(filePathEnc != NULL)
		{
			encoder(filePathEnc, filePathOut);
		}
		else
		{
			decoder(filePathDec, filePathOut);
		}
	}
	else
	{
		fprintf(stderr,"[E]: -e or -d option must be specified\n");
	}
	
	return 0;
}

void usage()
{
	printf(" -h or --help show this message and exit.\n");
	printf(" -e or --encode /path/to/file\n");
	printf(" -d or --decode /path/to/file\n");
	exit(EXIT_SUCCESS);
};
