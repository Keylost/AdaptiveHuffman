#include "huffman.hpp"

void printSimbol(uint32_t c)
{
	printf("%c\t", c);
	for(int i=0;i<32;i++)
	{
		if (c & (1<<(31-i))) printf("1");
		else printf("0");
	}
	printf("\n");
}

void printBinary(huffmanTree &tree)
{
	for(int i=0;i<tree.outputBufByteLen;i++)
	{
		uint32_t n = tree.outputBuf[i];
		for(int i=0;i<32;i++)
		{
			if (n & (1<<(31-i))) printf("1");
			else printf("0");
		}
		printf("\t");
	}
	
	uint32_t n = tree.outputBuf[tree.outputBufByteLen];
	for(int i=0;i<tree.outputBufBiteLen;i++)
	{
		if (n & (1<<(31-i))) printf("1");
		else printf("0");
	}
	
	printf("\n");
}

int encoder(const char* filePathInp, const char* filePathOut)
{
	unsigned char buffer[BUFFERSIZE];
	
	huffmanTree tree;
	
	size_t bytesReaded = 0;
	FILE *fpInp, *fpOut;
	uint32_t fileSize = 0;
	
	fpInp = fopen(filePathInp, "rb");
	fpOut = fopen(filePathOut, "wb");
	
	if(!fpInp)
	{
		printf("[E]: Can't open input file: %s\n", filePathInp);
	}
	if(!fpOut)
	{
		printf("[E]: Can't open output file: %s\n", filePathOut);
	}	
	
	fseek(fpInp, 0, SEEK_END); //перейти в конец файла
	fileSize = ftell(fpInp); //текущее смещение внутри файла
	rewind(fpInp); //в начало файла
	
	while((bytesReaded  = fread(buffer, 1, BUFFERSIZE, fpInp))>0)
	{
		printf("read %lu\n", bytesReaded);
		for(int i=0;i<bytesReaded;i++)
		{
			//printSimbol(buffer[i]);
			tree.add(buffer[i]);
			
			if(tree.outputBufByteLen > BUFFERSIZE-3)
			{
				tree.bufferFlush(fpOut);
			}
		}
	}
	
	if(bytesReaded == 0)
	{
		printBinary(tree);
		tree.bufferFlushWithEOF(fpOut);
		printf("[I]: Compression complete\n");
	}
	else
	{
		printf("[E]: Error occured while reading input stream\n");
	}
	
	fclose(fpInp);
	fclose(fpOut);
	
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

int decoder(const char* filePathInp, const char* filePathOut)
{
	return 0;
}
