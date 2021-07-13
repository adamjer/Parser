#include<iostream>

#include "Parser.h"

using namespace std;

int main()
{
	Parser* parser = new Parser();
	parser->loadCode("example.txt");
	//parser->executeCode();
	
	while (parser->nextCommand());
	
	delete parser;
	return 0;
}