
#ifndef HUFFMAN

#define HUFFMAN

#include <math.h>
#include <stdint.h>
#include <vector>
#include <cstddef>
#include <stdio.h>

#include <memory.h>

#define BUFFERSIZE	256
#define BIT_PER_SIMBOL 8 //сколько бит входной последовательности считать символом(только 8 сейчас поддерживается<(*_*)> )

class huffmanTreeNode;
/*
bool getBitOfChar(char *byte, uint8_t bit)
{
	return (byte&(1<<bit));
}
*/

void printSimbol(uint32_t c);
void addCharToBitArray(std::vector<bool> &array, char ascii);

class Simbol
{
	public:
	huffmanTreeNode *ref;
	
	Simbol()
	{
		ref = NULL;
	}
};

class huffmanTreeNode
{
	public:
	huffmanTreeNode* left;
	huffmanTreeNode* right;
	huffmanTreeNode* parent;
	
	uint32_t indexInNodeList; //huffmanTree::nodeList
	
	uint32_t weight;
	
	uint32_t symbolCode;
	uint32_t symbolLen;
	
	huffmanTreeNode(huffmanTreeNode* _parent)
	{
		left = NULL;
		right = NULL;
		parent = _parent;
		weight = 0;
		
		if(parent!=NULL)
		{
			symbolCode = parent->symbolCode;
			symbolLen = parent->symbolLen;
		}
		else
		{
			symbolLen = 0;
			symbolCode = 0;
		}
	}
	
	void symbolCodePushBack(uint32_t c)
	{
		if(c!=0) symbolCode = symbolCode|(1<<(symbolLen));
		symbolLen++;
	}
};

class huffmanTree
{
	public:
	Simbol *simbols;
	
	huffmanTreeNode** nodeList;
	uint32_t nodeListSize = 0;
	
	uint32_t *outputBuf;
	uint32_t outputBufByteLen;
	uint32_t outputBufBiteLen;
	
	huffmanTree() //init
	{
		outputBufByteLen = 0;
		outputBufBiteLen = 0;
		
		outputBuf = new uint32_t[BUFFERSIZE];
		memset(outputBuf, '\0', BUFFERSIZE);
		
		simbols = new Simbol[(int)pow(2,BIT_PER_SIMBOL)];
		
		nodeList = new huffmanTreeNode*[(int)pow(2,BIT_PER_SIMBOL)*2];
		
		nodeList[0] = new huffmanTreeNode(NULL);
		nodeList[0]->indexInNodeList = 0;
		nodeListSize = 1;
	}
	
	void printTree()
	{
		printf("tree:\n");
		for(unsigned i=0; i<nodeListSize; i++)
		{
			printf("%d\n", nodeList[i]->symbolLen);
		}
	}
	
	void add(unsigned char smb)
	{
		if(simbols[smb].ref == NULL)
		{
			/*
			 * Добавить в выходной поток код ESC-символа
			 */
			outputBufPushBack(nodeList[nodeListSize-1]->symbolCode, nodeList[nodeListSize-1]->symbolLen);
			
			/*
			 * Добавить в выходной поток ASCII-код символа
			 */
			outputBufPushBack((uint32_t)smb, 8);
			
			/*
			 * Создать лист под новый элемент и новый пустой элемент
			 */
			nodeList[nodeListSize] = new huffmanTreeNode(nodeList[nodeListSize-1]);
			nodeList[nodeListSize]->symbolCodePushBack(0); //левая ветвь с кодом 0
			//nodeList[nodeListSize]->indexInNodeList = (nodeList.size()-1);
			
			nodeList[nodeListSize+1] = new huffmanTreeNode(nodeList[nodeListSize-1]);
			nodeList[nodeListSize+1]->symbolCodePushBack(1); //правая ветвь с кодом 1
			//emptyNode->right->indexInNodeList = (nodeList.size()-1);
			
			simbols[smb].ref = nodeList[nodeListSize];
		}
		else
		{
			outputBufPushBack(simbols[smb].ref->symbolCode, simbols[smb].ref->symbolLen);
			//simbols[smb].ref->weight += 1;
		}
		
		updateTree(simbols[smb].ref);
		
	}
	
	void outputBufPushBack(uint32_t code, uint32_t len)
	{
		uint32_t bits = outputBufBiteLen+len;
		
		if(bits>32)
		{
			uint32_t codebkp = code;
			uint32_t left = (bits-32);
			code = code>>left;
			outputBuf[outputBufByteLen] = outputBuf[outputBufByteLen]|(code);
			outputBufByteLen += 1;
			codebkp = codebkp<<(32-left);
			outputBuf[outputBufByteLen] = outputBuf[outputBufByteLen]|(codebkp);
			outputBufBiteLen = left;
		}
		else
		{
			code = code<<(32-bits);
			outputBuf[outputBufByteLen] = outputBuf[outputBufByteLen]|(code);
			outputBufByteLen += bits/32;
			outputBufBiteLen = bits%32;
		}
	}
	
	void updateTree(huffmanTreeNode *node) ///TODO: Нуждается в адской оптимизации
	{		
		huffmanTreeNode *curNode = node;
		while(curNode->parent != NULL)
		{
			int ind = curNode->indexInNodeList;
			
			while(ind>0 && nodeList[ind]->weight == curNode->weight)
			{
				ind--;
			}
			
			if(ind==curNode->indexInNodeList || curNode->parent == nodeList[ind])
			{
				curNode->weight++;
			}
			else
			{
				curNode->weight++;
				//if(nodeList[ind]->left == NULL) //если лист, меняем листы
				{
					huffmanTreeNode *ndlp = nodeList[ind]->parent;
					if(nodeList[ind]->parent->left == nodeList[ind]) //левый сын
					{
						if(curNode->parent->left == curNode)
						{
							curNode->parent->left = nodeList[ind];
							nodeList[ind]->parent = curNode->parent;
							
							curNode->parent = ndlp;
							ndlp->left = curNode;
						}
						else
						{
							curNode->parent->right = nodeList[ind];
							nodeList[ind]->parent = curNode->parent;
							
							curNode->parent = ndlp;
							ndlp->left = curNode;							
						}
					}
					else
					{
						if(curNode->parent->left == curNode)
						{
							curNode->parent->left = nodeList[ind];
							nodeList[ind]->parent = curNode->parent;
							
							curNode->parent = ndlp;
							ndlp->right = curNode;
						}
						else
						{
							curNode->parent->right = nodeList[ind];
							nodeList[ind]->parent = curNode->parent;
							
							curNode->parent = ndlp;
							ndlp->right = curNode;							
						}
					}
					
					uint32_t indtmp = curNode->indexInNodeList;
					nodeList[ind]->indexInNodeList = curNode->indexInNodeList;
					nodeList[curNode->indexInNodeList] = nodeList[ind];
					
					curNode->indexInNodeList = ind;
					
					nodeList[ind] = curNode;
					
					curNode = nodeList[indtmp];
					
					uint32_t smc = nodeList[indtmp]->symbolCode;
					uint32_t sml = nodeList[indtmp]->symbolLen;
					
					nodeList[indtmp]->symbolCode = nodeList[ind]->symbolCode;
					nodeList[indtmp]->symbolCode = nodeList[ind]->symbolLen;
					
					nodeList[ind]->symbolCode = smc;
					nodeList[ind]->symbolCode = sml;
					
				}
				//else //иначе замена поддеревьев
				{
					
				}
				//swap()
				///нужно обновлять коды
			}
			curNode = curNode->parent;
		}
	}
	
	void bufferFlush(FILE *fp) //выводит полностью заполненные четырехбайтовые наборы в файл. Биты остаются нетронуты
	{
		printf("flushed %d\n", outputBufByteLen*4);
		
		fwrite(outputBuf, 4, outputBufByteLen, fp);
		if(outputBufBiteLen>0)
		{
			uint32_t tmp = outputBuf[outputBufByteLen];
			memset(outputBuf, '\0', BUFFERSIZE);
			outputBuf[0] = tmp;
		}
		outputBufByteLen = 0;
		
	}

	void bufferFlushWithEOF(FILE *fp) //выводит полностью заполненные четырехбайтовые наборы в файл, а также все биты. В конец добавляет ESC-символ.
	{
		outputBufPushBack(emptyNode->symbolCode, emptyNode->symbolLen);
		
		printf("flushed %d\n", outputBufByteLen*4);
		
		fwrite(outputBuf, 4, outputBufByteLen, fp);
		
		uint32_t tmp = outputBuf[outputBufByteLen];
		unsigned char *bt = ((unsigned char *)(&tmp));
		
		for(int i=0; i<outputBufBiteLen; i+=8)
		{
			fwrite(bt, 1, 1, fp);
			bt += 1;
		}
		
		outputBufByteLen = 0;
		outputBufBiteLen = 0;
		
		memset(outputBuf, '\0', BUFFERSIZE);
	}
	
};

int decoder(const char* filePathInp, const char* filePathOut);

int encoder(const char* filePathInp, const char* filePathOut);

#endif
