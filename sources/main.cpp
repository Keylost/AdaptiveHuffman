#include <stdio.h>
#include <getopt.h>

#include "huffman.hpp"

void usage();

int main(int argc, char **argv)
{
	char *filePath; //path to file
	
	const char* short_options = "he:d:";
	const struct option long_options[] =
	{
		{"help",no_argument,NULL,'e'},
		{"encode",required_argument,NULL,'e'},
		{"decode",required_argument,NULL,'d'},
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
					filePath = new char[strlen(optarg)+1];
					strcpy(filePath,optarg);
					break;
				};				
				case 'd':
				{
					filePath = new char[strlen(optarg)+1];
					strcpy(filePath,optarg);
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
	
	return 0;
}

void usage()
{
	printf(" -h or --help show this message and exit.\n");
	printf(" -e or --encode /path/to/file\n");
	printf(" -d or --decode /path/to/file\n");
	exit(EXIT_SUCCESS);
};
