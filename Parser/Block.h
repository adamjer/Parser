#pragma once
#include<string>
#include<map>
#include<vector>

#include"Operation.h"

struct Variable
{
	std::string name;
	int value;
};

class Block
{
public:
	Block();
	Block(bool b);
	~Block();

private:
	std::map<std::string, Variable> variables;
	std::vector<Block*> blocks;
	std::vector<Operation> operations;
	Block* parent;
	Block* root;
};

