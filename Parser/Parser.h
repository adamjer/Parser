#pragma once
#include<string>
#include<sstream>
#include<fstream>
#include<vector>
#include<regex>
#include<queue>

#include"Block.h"

enum ValueType
{
	Sign,
	Integer,
	Double,
	DeclaredGlobal,
	DeclaredOutside
};

class Parser
{
public:
	Parser();
	~Parser();

	void loadCode(std::string s);
	void executeCode();
	bool nextCommand();

private:
	int instructionPointer;
	std::map<std::string, int> etiquetes;
	std::map<std::string, int> variables;
	std::vector<std::string> code;

	void initTokens();
	bool isEtiquete(std::string&);
	void checkVariable(const std::string&);
	void execute(const std::string&);
	void trim(std::string&);
	bool findCommand(const std::string&, const std::string&);
	bool checkCondition(const std::string&);
	void skipIfBlock();
	std::vector<std::string> split(const std::string&, char);
	void parseOperation(const std::string&);
	void parse(std::queue<std::string>&, std::queue<std::string>&, std::string&, const std::vector<std::string>&);
	void adaptIfOperation(std::vector<std::string>&);
	bool isConditionalSign(const std::string&);
	int calculate(std::queue<std::string>&);
	bool evaluate(const std::string&, int, int);
	ValueType extractType(const std::string&);
	bool isNumeric(const std::string&);
	void parse(const std::vector<std::string>&, std::queue<std::string>&);
	void save(const std::string&, const int);
	double parseParameter(const std::string&);
	int getDataFromMatrix(const std::string&);
};