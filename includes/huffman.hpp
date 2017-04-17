//-o inputDec.txt -d out.txt
//-e input.txt -o out.txt


#ifndef HUFFMAN

#define HUFFMAN

#include <math.h>
#include <stdint.h>
#include <vector>
#include <cstddef>
#include <stdio.h>

#include <memory.h>

#include "timer.hpp"

#define BUFFERSIZE	256
#define BIT_PER_SIMBOL 8 //сколько бит входной последовательности считать символом(только 8 сейчас поддерживается<(*_*)> )

class huffmanTreeNode;
/*
bool getBitOfChar(char *byte, uint8_t bit)
{
	return (byte&(1<<bit));
}
*/

/*
 * Правая ветка - 1. Левая - 0. В левую ветку добавляется символ, в правую - ESC.
 * 
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
	unsigned char symbolValue;
	
	uint32_t weight;
	
	huffmanTreeNode(huffmanTreeNode* _parent)
	{
		left = NULL;
		right = NULL;
		parent = _parent;
		weight = 0;
	}
};

class huffmanTree
{
	public:
	Simbol *simbols;
	std::vector<huffmanTreeNode*> nodeList; 
	
	unsigned char *outputBuf;
	uint32_t outputBufByteLen;
	uint32_t outputBufBiteLen;
	
	huffmanTreeNode *rootNode; //корень дерева
	huffmanTreeNode *emptyNode; //пустой элемент дерева
	
	huffmanTree()
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
	
	void printTree()
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
	
	void printTreeCG(huffmanTreeNode *p,int level)
	{
		if(p)
		{
			printTreeCG(p->right,level + 1);
			for(int i = 0;i< level;i++) printf("|	");
			printf("%d\n", p->weight);
			printTreeCG(p->left,level + 1);
		}
	}
	
	void add(unsigned char smb)
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
		huffmanTreeNode* root = rootNode;
		//printTreeCG(root,0);
		//printf("-----------------------------------\n");
		//printTree();
		char c;
		//scanf("%c\n", &c);
	}

	void outputBufPushBack(huffmanTreeNode* node)
	{
		uint32_t code = 0;
		huffmanTreeNode* curNode = node;
		uint32_t len = 0;
		
		while(curNode->parent)
		{
			if(curNode->parent->right == curNode)
			{
				//outputBuf[outputBufByteLen] |= (1<<outputBufBiteLen);
				code |= (1<<len);
			}
			len++;
			/*
			outputBufBiteLen++;
			if(outputBufBiteLen==8)
			{
				outputBufByteLen++;
				outputBufBiteLen = 0;
			}
			* */
			curNode = curNode->parent;
		}
		
		for(int i=len-1;i>=0;i--)
		{
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
	
	void outputBufPushBack(unsigned char smb)
	{
		for(int i=0;i<8;i++)
		{
			outputBuf[outputBufByteLen] |= (((smb&(1<<i))>>i)<<outputBufBiteLen);
			outputBufBiteLen++;
			if(outputBufBiteLen==8)
			{
				outputBufByteLen++;
				outputBufBiteLen = 0;
			}
		}
	}
	
	void updateTree(huffmanTreeNode *node) ///TODO: Нуждается в адской оптимизации
	{
		huffmanTreeNode *curNode = node;
		while(curNode != NULL)
		{
			int ind = curNode->indexInNodeList;
			
			while(ind>0 && nodeList[ind-1]->weight == curNode->weight)
			{
				ind--;
			}
			
			printf("ind %d\n", ind);
			
			if(ind == curNode->indexInNodeList || curNode->parent == nodeList[ind] || ind == 0)
			{
				curNode->weight++;
			}
			else
			{
				printf("update+++++++++++++++++++++++++++++++++++++++++++\n");
				curNode->weight++;
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
				}
			}
			curNode = curNode->parent;
		}
	}
	
	void bufferFlush(FILE *fp) //выводит полностью заполненные четырехбайтовые наборы в файл. Биты остаются нетронуты
	{
		printf("flushed %d\n", outputBufByteLen*4);
		
		fwrite(outputBuf, 1, outputBufByteLen, fp);
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
		outputBufPushBack(emptyNode);
		printf("flushed %d\n", outputBufByteLen);
		
		//printSimbol(outputBuf[0]);
		
		fwrite((char*)outputBuf, 1, outputBufByteLen, fp);
		
		fwrite((char*)outputBuf+outputBufByteLen, 1, ((outputBufBiteLen>0) ? 1 : 0), fp);
		
		outputBufByteLen = 0;
		outputBufBiteLen = 0;
		
		memset(outputBuf, '\0', BUFFERSIZE);
	}
	
};

int decoder(const char* filePathInp, const char* filePathOut);

int encoder(const char* filePathInp, const char* filePathOut);

#endif
