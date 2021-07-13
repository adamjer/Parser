#include "Parser.h"


Parser::Parser()
{
	this->instructionPointer = 0;
	this->initTokens();
}

Parser::~Parser()
{
}

bool Parser::nextCommand()
{
	if (this->instructionPointer > this->code.size() - 1)
		return false;
	this->execute(this->code[instructionPointer]);
	return true;
}

void Parser::loadCode(std::string fileName)
{
	std::fstream file;
	file.open(fileName, std::ios::in);

	std::string etiquete;
	std::string line;
	if (file.is_open())
	{
		int IP = 0;
		while (!file.eof())
		{
			std::getline(file, line);
			this->trim(line);
			this->code.push_back(line);
			if (this->isEtiquete(line))
				this->etiquetes.emplace(line, IP);
			else
				this->checkVariable(line);

			IP++;
		}
		file.close();
	}
}

std::vector<std::string> Parser::split(const std::string& s, char delimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter))
	{
		tokens.push_back(token);
	}
	return tokens;
}

void Parser::checkVariable(const std::string& line)
{
	std::regex regex("\\s*INTEGER\\s+[A-Za-z_][A-Za-z0-9_]*\\s+=\\s+\\d+");

	if (std::regex_search(line, regex))
	{
		std::vector<std::string> temp = split(line, ' ');
		this->variables.emplace(temp[1], stoi(temp[3]));
	}
}

void Parser::trim(std::string& line)
{
	line = std::regex_replace(line, std::regex("^\\s+"), "");
	line = std::regex_replace(line, std::regex("\\s+"), " ");
	line = std::regex_replace(line, std::regex(","), "");
}

bool Parser::isEtiquete(std::string& line)
{
	std::regex regex("[A-Za-z_][A-Za-z0-9_]*:");
	std::smatch match;

	if (std::regex_search(line, match, regex))
	{
		line = match[0];
		line = line.substr(0, line.length() - 1);
		return true;
	}

	return false;
}

void Parser::initTokens()
{
}

void Parser::executeCode()
{
	instructionPointer = 0;
	while (instructionPointer < this->code.size() - 1)
	{
		this->execute(this->code[instructionPointer]);
	}
}

bool Parser::findCommand(const std::string& line, const std::string& pattern)
{
	std::regex regex(pattern);

	if (std::regex_search(line, regex))
		return true;
	return false;
}

void Parser::adaptIfOperation(std::vector<std::string>&	parsedOperation)
{
	for (int i = 1; i < parsedOperation.size(); i++)
	{
		if (parsedOperation[i][0] == '(')
			parsedOperation[i] = parsedOperation[i].substr(1);
		else if (parsedOperation[i][parsedOperation[i].size() - 1] == ')')
			parsedOperation[i] = parsedOperation[i].substr(0, parsedOperation[i].size() - 1);
	}
}

bool Parser::isConditionalSign(const std::string& sign)
{
	if (sign == ">" || sign == "<" || sign == "==" || sign == ">=" || sign == "<=" || sign == "!=")
		return true;
	return false;
}

void Parser::parse(std::queue<std::string>& rightSide, std::queue<std::string>& leftSide, std::string& sign, const std::vector<std::string>& parsedOperation)
{
	for (int i = 1; i < parsedOperation.size(); i++)
	{
		if (this->isConditionalSign(parsedOperation[i]))
			sign = parsedOperation[i];
		else
		{
			if (sign == "")
				leftSide.push(parsedOperation[i]);
			else
				rightSide.push(parsedOperation[i]);
		}
	}
}

bool Parser::isNumeric(const std::string& Integer)
{
	for (int i = 0; i < Integer.size(); i++)
		if (!(Integer[i] >= '0' && Integer[i] <= '9'))
			return false;
	return true;
}

ValueType Parser::extractType(const std::string& type)
{
	if (type == "+" || type == "-")
		return Sign;
	else if (this->variables.find(type) != this->variables.end())
		return DeclaredGlobal;
	else if (std::regex_match(type, std::regex("[0-9]*.[0-9]+")))
		return Double;
	else if (isNumeric(type))
		return Integer;
	else
		return DeclaredOutside;
}

int Parser::calculate(std::queue<std::string>& queue)
{
	int result = 0;
	std::string sign = "+";
	ValueType type;
	while (!queue.empty())
	{
		switch (type = extractType(queue.front()))
		{
		case Sign:
			sign = queue.front();
			break;
		case Integer:
			if (sign == "+")
				result += stoi(queue.front());
			else if (sign == "-")
				result -= stoi(queue.front());
			break;
		case DeclaredGlobal:
			if (sign == "+")
				result += this->variables[queue.front()];
			else if (sign == "-")
				result -= this->variables[queue.front()];
			break;
		case DeclaredOutside:
			//TODO get value from matrix
			break;
		}

		queue.pop();
	}
	return result;
}

bool Parser::evaluate(const std::string& sign, int leftSide, int rightSide)
{
	if (sign == ">")
	{
		if (leftSide > rightSide)
			return true;
		return false;
	}
	else if (sign == "<")
	{
		if (leftSide < rightSide)
			return true;
		return false;
	}
	else if (sign == ">=")
	{
		if (leftSide >= rightSide)
			return true;
		return false;
	}
	else if (sign == "<=")
	{
		if (leftSide <= rightSide)
			return true;
		return false;
	}
	else if (sign == "==")
	{
		if (leftSide == rightSide)
			return true;
		return false;
	}
	else if (sign == "!=")
	{
		if (leftSide != rightSide)
			return true;
		return false;
	}
	return false;
}

bool Parser::checkCondition(const std::string& line)
{
	std::queue<std::string> rightSide;
	std::queue<std::string> leftSide;
	std::string sign;

	std::vector<std::string> parsedOperation = this->split(line, ' ');
	this->adaptIfOperation(parsedOperation);
	this->parse(rightSide, leftSide, sign, parsedOperation);

	return evaluate(sign, calculate(leftSide), calculate(rightSide));
}

void Parser::skipIfBlock()
{
	while (!findCommand(this->code[instructionPointer], "ENDIF"))
	{
		instructionPointer++;
	}
	instructionPointer++;
}

void Parser::parse(const std::vector<std::string>& parsedOperation, std::queue<std::string>& rightSide)
{
	bool isReady = false;
	for (int i = 2; i < parsedOperation.size(); i++)
	{
		rightSide.push(parsedOperation[i]);
	}
}

void Parser::save(const std::string& variable, const int result)
{
	this->variables[variable] = result;
}

void Parser::parseOperation(const std::string& line)
{
	std::queue<std::string> rightSide;

	std::vector<std::string> parsedOperation = this->split(line, ' ');
	this->parse(parsedOperation, rightSide);

	this->save(parsedOperation[0], calculate(rightSide));
}

double Parser::parseParameter(const std::string& value)
{
	double result = 0;
	ValueType type = this->extractType(value);
	switch (type)
	{
	case Integer:
		result += (double)stoi(value);
		break;
	case Double:
		result += stod(value);
		break;
	case DeclaredGlobal:
		result += this->variables[value];
		break;
	}
	return result;
}

int Parser::getDataFromMatrix(const std::string& field)
{
	int index = stoi(field.substr(1));
	if (field[0] == 'P')
	{
		//call pMATRIX[index]
		return 0;
	}
	else if (field[0] == 'J')
	{
		//call jMATRIX[index]
		return 0;
	}
}

void Parser::execute(const std::string& line)
{
	std::vector<std::string> parsedCommand;
	if (this->findCommand(line, "GOTO"))
	{
		parsedCommand = split(line, ' ');
		instructionPointer = this->etiquetes[parsedCommand[1]];
	}
	else if (this->findCommand(line, "MOVJ"))
	{
		parsedCommand = split(line, ' ');
		getDataFromMatrix(parsedCommand[1]);
		//MOVJ(getDataFromMatrix(parsedCommand[1]));
		instructionPointer++;
	}
	else if (this->findCommand(line, "MOVS"))
	{
		parsedCommand = split(line, ' ');
		//MOVS(parsedCommand[1]);
		instructionPointer++;
	}
	else if (this->findCommand(line, "MOVMVS"))
	{
		parsedCommand = split(line, ' ');
		//MOVMVS(parsedCommand[1]);
		instructionPointer++;
	}
	else if (this->findCommand(line, "ADDX"))
	{
		parsedCommand = split(line, ' ');
		//ADDX(parsedCommand[1], parseParameter(parsedCommand[2]));
		instructionPointer++;
	}
	else if (this->findCommand(line, "ADDY"))
	{
		parsedCommand = split(line, ' ');
		//ADDY(parsedCommand[1], parseParameter(parsedCommand[2]));
		instructionPointer++;
	}
	else if (this->findCommand(line, "ADDZ"))
	{
		parsedCommand = split(line, ' ');
		//ADDZ(parsedCommand[1], parseParameter(parsedCommand[2]));
		instructionPointer++;
	}
	else if (this->findCommand(line, "IF (.*)"))
	{
		if (checkCondition(line))
			instructionPointer++;
		else
			skipIfBlock();
	}
	else if (this->findCommand(line, "^[A-Za-z][A-Za-z0-9]*\\s+([=+-]\\s+[A-Za-z][A-Za-z0-9]*)+")) //operation
	{
		this->parseOperation(line);
		instructionPointer++;
	}
	else
		instructionPointer++;
}