#include "huffman.hpp"

void printSimbol(unsigned char c)
{
	printf("%c\t", c);
	for(int i=0;i<8;i++)
	{
		if (c & (1<<(7-i))) printf("1");
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
				outputFileSize += tree.outputBufByteLen;
				tree.bufferFlush(fpOut);
			}
		}
	}
	tim.stop();
	
	if(bytesReaded == 0)
	{
		outputFileSize += tree.outputBufByteLen;
		outputFileSize += (tree.outputBufBiteLen>0 ? 1 : 0);
		tree.bufferFlushWithEOF(fpOut);
		printf("[I]: Compression complete\n");
		printf("[INFO]:\n");
		printf("Input file size:   %d bytes\n", fileSize);
		printf("Output file size:  %d bytes\n", outputFileSize);
		printf("Compression ratio: %f\n", (double)fileSize/outputFileSize);
		printf("Compression time:  %lu ms\n", tim.get(rtimer_ms));
		
		//tree.printTreeCG(tree.rootNode,0);
		//tree.printTree();
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
	
	huffmanTreeNode *curNode = tree.rootNode;
	huffmanTreeNode *prevNode = tree.rootNode;
	
	bool asciiState = true;
	unsigned char asciiSymbol = '\0';
	uint32_t asciiLen = 0;
		
	while((bytesReaded  = fread(buffer, 1, BUFFERSIZE, fpInp))>0)
	{
		for(int byte=0;byte<bytesReaded;byte++)
		{
			for(int bit=0;bit<8;bit++)
			{
				if(asciiState)
				{
					if( (buffer[byte]&(1<<bit)) )
					{
						asciiSymbol |= (1<<asciiLen);
					}
					//printSimbol(asciiSymbol);
					asciiLen++;
					if(asciiLen == 8)
					{
						asciiState = false;
						asciiLen = 0;
						outBuf.push_back(asciiSymbol);
						tree.add(asciiSymbol);
						//printSimbol(asciiSymbol);
						asciiSymbol = '\0';
					}
					continue;
				}
				
				prevNode = curNode;
				if( (buffer[byte]&(1<<bit)) )
				{
					curNode = curNode->right;
				}
				else
				{
					curNode = curNode->left;
				}
				if(!curNode->left)
				{
					if(curNode->weight)
					{
						outBuf.push_back(curNode->symbolValue);
						tree.add(curNode->symbolValue);
						if(tree.outputBufByteLen > BUFFERSIZE-3) tree.outputBufByteLen = 0;
						curNode = tree.rootNode;
						prevNode = tree.rootNode;
					}
					else
					{
						asciiState = true;
						curNode = tree.rootNode;
						prevNode = tree.rootNode;
					}
				}
			}
		}
		if(outBuf.size()>256)
		{
			fwrite(outBuf.data(), 1, outBuf.size(), fpOut);
			outBuf.clear();
		}
	}
	
	if(bytesReaded == 0)
	{
		if(outBuf.size()>0)
		{
			fwrite(outBuf.data(), 1, outBuf.size(), fpOut);
			outBuf.clear();
		}
		printf("[I]: Decompression complete\n");
	}
	else
	{
		printf("[E]: Error occured while reading input stream\n");
	}
	
	fclose(fpInp);
	fclose(fpOut);		
	return 0;
}

huffmanTree::huffmanTree()
{
	outputBufByteLen = 0;
	outputBufBiteLen = 0;
	
	outputBuf = new unsigned char[BUFFERSIZE];
	memset(outputBuf, '\0', BUFFERSIZE);
	
	simbols = new Simbol[256];
	
	rootNode = new huffmanTreeNode(NULL);
	nodeList.push_back(rootNode);
	rootNode->indexInNodeList = (nodeList.size()-1);
	
	emptyNode = rootNode;
}

void huffmanTree::printTree()
{
	int maxLen = 0;
	printf("tree: size %lu \n", nodeList.size());
	for(unsigned i=0; i<nodeList.size(); i++)
	{
		printf("%d	", nodeList[i]->weight);
		
		huffmanTreeNode* cur=nodeList[i];
		int lenTmp = 0;
		while(cur->parent)
		{
			lenTmp++;
			cur = cur->parent;
		}
		if(lenTmp>maxLen) maxLen = lenTmp;
	}
	printf("\n");
	printf("maxlen %d\n", maxLen);
}


void huffmanTree::printTreeCG(huffmanTreeNode *p,int level)
{
	if(p)
	{
		printTreeCG(p->right,level + 1);
		for(int i = 0;i< level;i++) printf("|	");
		printf("%d\n", p->weight);
		printTreeCG(p->left,level + 1);
	}
}

void huffmanTree::add(unsigned char smb)
{
	if(simbols[smb].ref == NULL)
	{
		/*
		 * Добавить в выходной поток код ESC-символа
		 */
		outputBufPushBack(emptyNode);
			
		/*
		 * Добавить в выходной поток ASCII-код символа
		 */
		outputBufPushBack(smb);
			
		/*
		 * Создать лист под новый элемент и новый пустой элемент
		 */
		emptyNode->left = new huffmanTreeNode(emptyNode);
		nodeList.push_back(emptyNode->left);
		emptyNode->left->indexInNodeList = (nodeList.size()-1);
		
		emptyNode->right = new huffmanTreeNode(emptyNode);
		nodeList.push_back(emptyNode->right);
		emptyNode->right->indexInNodeList = (nodeList.size()-1);
		
		simbols[smb].ref = emptyNode->left;
		simbols[smb].ref->symbolValue = smb;
		emptyNode = emptyNode->right; //правый лист становится новым пустым элементом
	}
	else
	{
		outputBufPushBack(simbols[smb].ref);
	}
	
	updateTree(simbols[smb].ref);
	//huffmanTreeNode* root = rootNode;
	//printTreeCG(root,0);
	//printf("-----------------------------------\n");
	//printTree();
	char c;
	//scanf("%c\n", &c);
}

void huffmanTree::outputBufPushBack(huffmanTreeNode* node)
{
	uint32_t code = 0;
	huffmanTreeNode* curNode = node;
	uint32_t len = 0;
	
	while(curNode->parent)
	{
		if(curNode->parent->right == curNode)
		{
			code |= (1<<len);
		}
		len++;
		curNode = curNode->parent;
	}
	
	for(int i=len-1;i>=0;i--)
	{
		outputBuf[outputBufByteLen] &= ~(1<<outputBufBiteLen);
		outputBuf[outputBufByteLen] |= ((code>>i)<<outputBufBiteLen);
		outputBufBiteLen++;
		if(outputBufBiteLen==8)
		{
			outputBufByteLen++;
			outputBufBiteLen = 0;
		}
	}
	
	return;
}

void huffmanTree::outputBufPushBack(unsigned char smb)
{
	for(int i=0;i<8;i++)
	{
		outputBuf[outputBufByteLen] &= ~(1<<outputBufBiteLen);
		outputBuf[outputBufByteLen] |= (((smb&(1<<i))>>i)<<outputBufBiteLen);
		outputBufBiteLen++;
		if(outputBufBiteLen==8)
		{
			outputBufByteLen++;
			outputBufBiteLen = 0;
		}
	}
}

void huffmanTree::updateTree(huffmanTreeNode *node)
{
	huffmanTreeNode *curNode = node;
	while(curNode != NULL)
	{
		int ind = curNode->indexInNodeList;
		
		while(ind>0 && nodeList[ind-1]->weight == curNode->weight)
		{
			ind--;
		}
		
		if(ind == curNode->indexInNodeList || curNode->parent == nodeList[ind] || ind == 0)
		{
			curNode->weight++;
		}
		else
		{
			curNode->weight++;
			huffmanTreeNode *nodeToChg = nodeList[ind];
			
			huffmanTreeNode *nodeToChg_parent = nodeToChg->parent;
			
			nodeToChg->indexInNodeList = curNode->indexInNodeList;
			curNode->indexInNodeList = ind;
			
			if(curNode->parent->left == curNode) curNode->parent->left = nodeToChg;
			else curNode->parent->right = nodeToChg;
			
			if(nodeToChg->parent->left == nodeToChg) nodeToChg->parent->left = curNode;
			else nodeToChg->parent->right = curNode;
			
			nodeList[ind] = curNode;
			nodeList[nodeToChg->indexInNodeList] = nodeToChg;
			
			nodeToChg->parent = curNode->parent;
			curNode->parent = nodeToChg_parent;
		}
		curNode = curNode->parent;
	}
}

void huffmanTree::bufferFlush(FILE *fp)
{
	fwrite(outputBuf, 1, outputBufByteLen, fp);
	if(outputBufBiteLen>0)
	{
		uint32_t tmp = outputBuf[outputBufByteLen];
		memset(outputBuf, '\0', BUFFERSIZE);
		outputBuf[0] = tmp;
	}
	outputBufByteLen = 0;
	
}

void huffmanTree::bufferFlushWithEOF(FILE *fp) //выводит полностью заполненные четырехбайтовые наборы в файл, а также все биты. В конец добавляет ESC-символ.
{
	outputBufPushBack(emptyNode);
	
	fwrite((char*)outputBuf, 1, outputBufByteLen, fp);
	
	fwrite((char*)outputBuf+outputBufByteLen, 1, ((outputBufBiteLen>0) ? 1 : 0), fp);
	
	outputBufByteLen = 0;
	outputBufBiteLen = 0;
	
	memset(outputBuf, '\0', BUFFERSIZE);
}
