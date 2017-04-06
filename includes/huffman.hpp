
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
	std::vector<huffmanTreeNode*> nodeList; 
	
	uint32_t *outputBuf;
	uint32_t outputBufByteLen;
	uint32_t outputBufBiteLen;
	
	huffmanTreeNode *rootNode; //корень дерева
	huffmanTreeNode *emptyNode; //пустой элемент дерева
	
	huffmanTree()
	{
		outputBufByteLen = 0;
		outputBufBiteLen = 0;
		
		outputBuf = new uint32_t[BUFFERSIZE];
		memset(outputBuf, '\0', BUFFERSIZE);
		
		simbols = new Simbol[(int)pow(2,BIT_PER_SIMBOL)];
		
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
	
	//int cnt = 0;
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
			outputBufPushBack((uint32_t)smb, 8);
			
			/*
			 * Создать лист под новый элемент и новый пустой элемент
			 */
			emptyNode->left = new huffmanTreeNode(emptyNode);
			emptyNode->left->symbolCodePushBack(0); //левая ветвь с кодом 0
			//emptyNode->left->weight = 1; //в левый лист помещается новый символ
			nodeList.push_back(emptyNode->left);
			emptyNode->left->indexInNodeList = (nodeList.size()-1);
			
			emptyNode->right = new huffmanTreeNode(emptyNode);
			emptyNode->right->symbolCodePushBack(1); //правая ветвь с кодом 1
			nodeList.push_back(emptyNode->right);
			emptyNode->right->indexInNodeList = (nodeList.size()-1);
			
			simbols[smb].ref = emptyNode->left;
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
		uint32_t code=0;
		huffmanTreeNode* curNode = node;
		uint32_t len = 0;
		
		while(curNode->parent)
		{
			if(node->parent->right == node)
			{
				code = code|(1<<len);
			}
			len++;
			curNode = curNode->parent;
		}

		
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
		while(curNode != NULL)
		{
			int ind = curNode->indexInNodeList;// == 0 ? 0 : (curNode->indexInNodeList);
			
			while(ind>0 && nodeList[ind-1]->weight == curNode->weight)
			{
				ind--;
			}
			
			printf("ind %d\n", ind);
			
			if(ind == curNode->indexInNodeList || curNode->parent == nodeList[ind] || ind == 0) //nodeList[ind]->weight > curNode->weight
			{
				curNode->weight++;
			}
			else
			{
				printf("update+++++++++++++++++++++++++++++++++++++++++++\n");
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
					
					//curNode = nodeList[indtmp];
					
					uint32_t smc = nodeList[indtmp]->symbolCode;
					uint32_t sml = nodeList[indtmp]->symbolLen;
					
					nodeList[indtmp]->symbolCode = nodeList[ind]->symbolCode;
					nodeList[indtmp]->symbolCode = nodeList[ind]->symbolLen;
					
					nodeList[ind]->symbolCode = smc;
					nodeList[ind]->symbolCode = sml;
					
				}
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
