#include "huffman.hpp"


int encoder(const char* filePath)
{
	unsigned char buffer[ENCODER_BUFFER_SIZE];
	
	huffmanTree tree;
	
	ssize_t bytesReaded = 0;
	FILE *fp;
	uint32_t fileSize = 0;
	
	fp = fopen(filePath, "rb");
	
	fseek(fp, 0, SEEK_END); //перейти в конец файла
	fileSize = ftell(fp); //текущее смещение внутри файла
	rewind(fp); //в начало файла
	
	while((bytesReaded  = read(fp, buffer, ENCODER_BUFFER_SIZE))>0)
	{
		for(int i=0;i<bytesReaded;i++)
		{	
			tree.add(buffer[i]);
			
			if(simbols[buffer[i]].ref != NULL)
			{
				huffmanTreeNode *node = new huffmanTreeNode();
				simbols[buffer[i]].ref = node;
				
			}
			else
			{
				
			}
		}
	}
	
	if(bytesReaded == 0)
	{
		printf("[I]: Compression complete\n");
	}
	else
	{
		printf("[E]: Error occured while reading input stream\n");
	}
	
	fclose(fp);
}
