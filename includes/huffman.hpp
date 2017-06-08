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
 * Правая ветка - 1. Левая - 0. В левую ветку добавляется символ, в правую - ESC.
 * 
 */

void printSimbol(unsigned char c);

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
	
	huffmanTree();	
	void printTree();
	void printTreeCG(huffmanTreeNode *p,int level);
	void add(unsigned char smb);
	void outputBufPushBack(huffmanTreeNode* node);
	void outputBufPushBack(unsigned char smb);
	void updateTree(huffmanTreeNode *node);
	void bufferFlush(FILE *fp); //выводит полностью заполненные байтовые наборы в файл. Биты остаются нетронуты
	void bufferFlushWithEOF(FILE *fp); //выводит полностью заполненные байтовые наборы в файл, а также все биты. В конец добавляет ESC-символ.
	
};

int decoder(const char* filePathInp, const char* filePathOut);

int encoder(const char* filePathInp, const char* filePathOut);

#endif
