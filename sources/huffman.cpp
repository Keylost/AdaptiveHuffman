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
	RTimer tim;
	
	size_t bytesReaded = 0;
	FILE *fpInp, *fpOut;
	uint32_t fileSize = 0;
	uint32_t outputFileSize = 0;
	
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
	
	tim.start();
	while((bytesReaded  = fread(buffer, 1, BUFFERSIZE, fpInp))>0)
	{
		for(int i=0;i<bytesReaded;i++)
		{
			tree.add(buffer[i]);
			
			if(tree.outputBufByteLen > BUFFERSIZE-3)
			{
				outputFileSize += tree.outputBufByteLen*4;
				tree.bufferFlush(fpOut);
			}
		}
	}
	tim.stop();
	
	if(bytesReaded == 0)
	{
		outputFileSize += tree.outputBufByteLen*4;
		outputFileSize += tree.outputBufBiteLen/8;
		outputFileSize += (tree.outputBufBiteLen%8>0 ? 1 : 0);
		tree.bufferFlushWithEOF(fpOut);
		printf("[I]: Compression complete\n");
		printf("[INFO]:\n");
		printf("Input file size:   %d bytes\n", fileSize);
		printf("Output file size:  %d bytes\n", outputFileSize);
		printf("Compression ratio: %f\n", (double)fileSize/outputFileSize);
		printf("Compression time:  %lu ms\n", tim.get(rtimer_ms));
	}
	else
	{
		printf("[E]: Error occured while reading input stream\n");
	}
	
	//tree.printTree();
	//huffmanTreeNode* root = tree.rootNode;
	//tree.printTreeCG(root,0);
	
	fclose(fpInp);
	fclose(fpOut);
	
	return 0;
}

int decoder(const char* filePathInp, const char* filePathOut)
{
	unsigned char buffer[BUFFERSIZE];
	std::vector<unsigned char> outBuf;
	
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
		uint32_t curByte = 0;
		uint32_t curBite = 0;
		huffmanTreeNode *curNode = rootNode;
		huffmanTreeNode *prevNode = rootNode;
		
		for(int byte=0;byte<bytesReaded;byte++)
		{
			//
			for(int bit=0;bit<8;bit++)
			{
				prevNode = curNode;
				if( (buffer[curByte]&(1<<bit)) )
				{
					curNode = curNode->right;
				}
				else
				{
					curNode = curNode->left;
				}
				if(!curNode)
				{
					if(prevNode->weight)
					{
						outBuf.push_back(prevNode->symbolValue);
						tree.add(prevNode->symbolValue);
						if(tree.outputBufByteLen > BUFFERSIZE-3) tree.outputBufByteLen = 0;
						curNode = rootNode;
						prevNode = rootNode;
					}
					else
					{
						if( (bytesReaded-byte)*8-bit < 8 )
						{
							
						}
						else
						{
							
						}
					}
				}
			}
			byte++;
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
	
	tree.printTree();
	huffmanTreeNode* root = tree.rootNode;
	tree.printTreeCG(root,0);
	
	fclose(fpInp);
	fclose(fpOut);		
	return 0;
}
