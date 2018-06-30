#pragma once
#include <string>
#include <vector>


class CPattern
{
	struct SChanges {
		std::string m_PatternChar;
		std::string m_newWordChar;
		std::string m_newVariable;
	};

public:
	CPattern();
	~CPattern();
	std::string getPattern();
	void setAlphabet(std::vector<std::string> _sAlphabet);
	bool checkPattern(std::string _newString);
private:
	std::vector<std::string> m_sPattern;
	std::vector<SChanges> m_pChanges;
	char m_cVariable;
	std::vector<std::string> m_vAlphabet;
	void setVariable();
	std::string getNewVariable();
	bool isStringInAlphabet(std::string _newString);
};

