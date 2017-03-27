
#ifndef HUFFMAN

#define HUFFMAN

#include <math.h>
#include <stdint.h>
#include <vector>
#include <cstddef>
#include <stdio.h>
//#include <unistd.h>

#define BUFFERSIZE	256
#define BIT_PER_SIMBOL 8 //сколько бит входной последовательности считать символом(только 8 сейчас поддерживается<(*_*)> )

class huffmanTreeNode;
/*
bool getBitOfChar(char *byte, uint8_t bit)
{
	return (byte&(1<<bit));
}
*/

void addCharToBitArray(std::vector<bool> &array, char ascii);

/*
class outputBuffer
{
	public:
	char *buf;
	uint32_t curLen; //текущая длина буфера в битах
	
	outputBuffer()
	{
		buf = new char[ENCODER_BUFFER_SIZE];
		curLen = 0;
	}
	
	void addToBuffer(uint32_t simCode, uint32_t lenCode)
	{
		uint32_t bytes = curLen/8;
		char *curByte = buf+bytes;
	}
	
};
*/

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
	
	uint32_t weight;

	std::vector<bool> symbolCode;	
	
	huffmanTreeNode(huffmanTreeNode* _parent)
	{
		left = NULL;
		right = NULL;
		parent = _parent;
		weight = 0;
		
		if(parent!=NULL)
		{
			symbolCode.insert(symbolCode.begin(), parent->symbolCode.begin(), parent->symbolCode.end());
		}
	}
};

class huffmanTree
{
	public:
	Simbol *simbols;
	
	std::vector<bool> outputBuf;
	
	huffmanTreeNode *rootNode; //корень дерева
	huffmanTreeNode *emptyNode; //пустой элемент дерева
	
	huffmanTree()
	{
		simbols = new Simbol[(int)pow(2,BIT_PER_SIMBOL)];
		rootNode = new huffmanTreeNode(NULL);
		emptyNode = rootNode;
		emptyNode->symbolCode.push_back(0);
	}
	
	void add(unsigned char smb)
	{
		if(simbols[smb].ref == NULL)
		{
			/*
			 * Добавить в выходной поток код ESC-символа
			 */
			outputBuf.insert(outputBuf.end(), emptyNode->symbolCode.begin(), emptyNode->symbolCode.end());
			/*
			 * Добавить в выходной поток ASCII-код символа
			 */
			addCharToBitArray(outputBuf, smb);
			
			/*
			 * Создать лист под новый элемент и новый пустой элемент
			 */
			emptyNode->left = new huffmanTreeNode(emptyNode);
			emptyNode->left->symbolCode.push_back(0); //левая ветвь с кодом 0
			emptyNode->left->weight = 1; //в левый лист помещается новый символ
			
			emptyNode->right = new huffmanTreeNode(emptyNode);
			emptyNode->right->symbolCode.push_back(1); //правая ветвь с кодом 1
					
			simbols[smb].ref = emptyNode->left;
			emptyNode = emptyNode->right; //правый лист становится новым пустым элементом
		}
		else
		{
			outputBuf.insert(outputBuf.end(), simbols[smb].ref->symbolCode.begin(), simbols[smb].ref->symbolCode.end());
			simbols[smb].ref->weight += 1;
		}
	}
	
	void updateTree()
	{
		
	}
};

int decoder(const char* filePath);

int encoder(const char* filePath);

#endif
