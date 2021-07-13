#include "Block.h"



Block::Block()
{
	this->parent = NULL;
	this->root = NULL;
}

Block::Block(bool isRoot)
{
	this->parent = NULL;
	this->root = this;
}


Block::~Block()
{
	if (this->parent != NULL)
		delete this->parent;
	if (this->root != NULL)
		delete this->root;
}
