#pragma once

#include <string>
#include <vector>

class CGenerator
{
public:
	CGenerator(std::vector<std::string> _pattern, std::vector<std::string> _alphabet);

	//Generates the words and returns them
	//One letter from the alphabet is one string --> one word is one vector of strings
	//the algorithm needs to know what one "element" is to see if it is a variable
	std::vector<std::vector<std::string> > GenerateWords();

	//like the GenerateWords function but writes the list into a file
	void GenerateFile(std::string _fileName);

private:
	std::vector<std::string> m_Pattern;
	std::vector<std::string> m_Alphabet;
};