
#ifndef HUFFMAN

#define HUFFMAN

#include <math.h>
#include <stdint.h>
#include <vector>
#include <cstddef>
#include <stdio.h>

#include <memory.h>
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

void printSimbol(unsigned char c);
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
	
	uint32_t symbolCode;
	uint32_t symbolLen;
	//std::vector<bool> symbolCode;	
	
	huffmanTreeNode(huffmanTreeNode* _parent)
	{
		left = NULL;
		right = NULL;
		parent = _parent;
		weight = 0;
		
		if(parent!=NULL)
		{
			//symbolCode.insert(symbolCode.begin(), parent->symbolCode.begin(), parent->symbolCode.end());
			symbolCode = parent->symbolCode;
			symbolLen = parent->symbolLen;
		}
		else
		{
			symbolLen = 0;
			symbolCode = 0;
		}
	}
	
	void symbolCodePushBack(uint8_t c)
	{
		if(c!=0) symbolCode = symbolCode|(1<<(symbolLen));
		symbolLen++;
	}
};

class huffmanTree
{
	public:
	Simbol *simbols;
	
	//std::vector<bool> outputBuf;
	
	unsigned char *outputBuf;
	uint32_t outputBufByteLen = 0;
	uint32_t outputBufBiteLen = 0;
	
	huffmanTreeNode *rootNode; //корень дерева
	huffmanTreeNode *emptyNode; //пустой элемент дерева
	
	huffmanTree()
	{
		outputBuf = new unsigned char[BUFFERSIZE];
		memset(outputBuf, '\0', BUFFERSIZE);
		
		simbols = new Simbol[(int)pow(2,BIT_PER_SIMBOL)];
		rootNode = new huffmanTreeNode(NULL);
		emptyNode = rootNode;
		//emptyNode->symbolCodePushBack(0);
	}
	
	void add(unsigned char smb)
	{
		if(simbols[smb].ref == NULL)
		{
			/*
			 * Добавить в выходной поток код ESC-символа
			 */
			//outputBuf.insert(outputBuf.end(), emptyNode->symbolCode.begin(), emptyNode->symbolCode.end());
			//printSimbol(emptyNode->symbolCode);
			//outputBufPushBack(emptyNode->symbolCode, emptyNode->symbolLen);
			
			/*
			 * Добавить в выходной поток ASCII-код символа
			 */
			//addCharToBitArray(outputBuf, smb);
			outputBufPushBack((uint32_t)smb, 8);
			
			/*
			 * Создать лист под новый элемент и новый пустой элемент
			 */
			emptyNode->left = new huffmanTreeNode(emptyNode);
			emptyNode->left->symbolCodePushBack(0); //левая ветвь с кодом 0
			emptyNode->left->weight = 1; //в левый лист помещается новый символ
			
			emptyNode->right = new huffmanTreeNode(emptyNode);
			emptyNode->right->symbolCodePushBack(1); //правая ветвь с кодом 1
					
			simbols[smb].ref = emptyNode->left;
			emptyNode = emptyNode->right; //правый лист становится новым пустым элементом
		}
		else
		{
			outputBufPushBack(simbols[smb].ref->symbolCode, simbols[smb].ref->symbolLen);
			simbols[smb].ref->weight += 1;
		}
	}
	
	void outputBufPushBack(uint32_t code, uint32_t len)
	{
		uint32_t bits = outputBufBiteLen+len;
		
		printSimbol((uint8_t)code);
		
		code = ((uint32_t)code)<<24;
		
		outputBuf[outputBufByteLen] = outputBuf[outputBufByteLen]|(code);
		
		printSimbol(emptyNode->symbolCode);
		printSimbol(code);
		
		outputBufByteLen += bits/8;
		outputBufBiteLen = bits%8;
		
		/*
		if(bits>=16)
		{
			outputBuf[outputBufByteLen] = outputBuf[outputBufByteLen]|(((uint16_t)code)<<16-outputBufBiteLen-len);
			outputBufByteLen += 1;
			outputBufBiteLen = bits%8;			
		}
		else if(bits>=8)
		{
			outputBuf[outputBufByteLen] = outputBuf[outputBufByteLen]|(((uint16_t)code)<<16-outputBufBiteLen-len);
			outputBufByteLen += bits/8;
			outputBufBiteLen = bits%8;
		}
		else
		{
			outputBuf[outputBufByteLen] = outputBuf[outputBufByteLen]|(((uint8_t)code)<<(8-outputBufBiteLen-len));
			outputBufBiteLen = bits;
		}
		*/
	}
	
	void updateTree()
	{
		
	}
};

int decoder(const char* filePath);

int encoder(const char* filePath);

#endif
