#include "huffman.hpp"

void printSimbol(unsigned char c)
{
	//printf("%c\t", c);
	for(int i=0;i<8;i++)
	{
		if (c & 1) printf("1");
		else printf("0");
		c >>= 1;
	}
	printf("\n");
}

void printBinary(huffmanTree &tree)
{
	for(int i=0;i<tree.outputBufByteLen;i++)
	{
		unsigned char n = tree.outputBuf[i];
		for(int j=0;j<8;j++)
		{
			if (n & 1) printf("1");
			else printf("0");
			n >>= 1;
		}
		printf("\t");
	}
	
	unsigned char n = tree.outputBuf[tree.outputBufByteLen];
	for(int i=0;i<tree.outputBufBiteLen;i++)
	{
		if (n & 1) printf("1");
		else printf("0");
		n >>= 1;
	}
	
	printf("\n");
}

int encoder(const char* filePath)
{
	unsigned char buffer[BUFFERSIZE];
	
	huffmanTree tree;
	
	size_t bytesReaded = 0;
	FILE *fp;
	uint32_t fileSize = 0;
	
	fp = fopen(filePath, "rb");
	
	fseek(fp, 0, SEEK_END); //перейти в конец файла
	fileSize = ftell(fp); //текущее смещение внутри файла
	rewind(fp); //в начало файла
	
	while((bytesReaded  = fread(buffer, 1, 1, fp))>0)
	{
		for(int i=0;i<bytesReaded;i++)
		{
			//printSimbol(buffer[i]);
			
			tree.add(buffer[i]);
			
			printBinary(tree);
			
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
	
	return 0;
}


void addCharToBitArray(std::vector<bool> &array, char ascii)
{
	array.push_back((ascii&(1<<0)) > 0 ? 1 : 0);
	array.push_back(ascii&(1<<1)  > 0 ? 1 : 0);
	array.push_back(ascii&(1<<2)  > 0 ? 1 : 0);
	array.push_back(ascii&(1<<3)  > 0 ? 1 : 0);
	array.push_back(ascii&(1<<4)  > 0 ? 1 : 0);
	array.push_back(ascii&(1<<5)  > 0 ? 1 : 0);
	array.push_back(ascii&(1<<6)  > 0 ? 1 : 0);
	array.push_back(ascii&(1<<7)  > 0 ? 1 : 0);
}

int decoder(const char* filePath)
{
	return 0;
}
