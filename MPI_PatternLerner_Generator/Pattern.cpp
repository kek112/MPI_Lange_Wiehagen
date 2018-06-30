#include "Pattern.h"



CPattern::CPattern() : m_cVariable(' ')
{
}


CPattern::~CPattern()
{
}

std::string CPattern::getPattern()
{
	std::string returnString;
	for (unsigned int i = 0; i < m_sPattern.size(); i++)
	{
		returnString.append(m_sPattern.at(i));
	}
	return returnString;
}

void CPattern::setAlphabet(std::vector<std::string> _sAlphabet)
{
	m_vAlphabet = _sAlphabet;
	setVariable();
}

void CPattern::setVariable()
{
	bool bCharInAlphabet = false;
	for (int i = 97; i < 123; i++)
	{
		bCharInAlphabet = false;
		for (int j = 0; j < m_vAlphabet.size(); j++)
		{
			if (i == m_vAlphabet.at(j).at(0))
			{
				bCharInAlphabet = true;
			}
		}

		if (!bCharInAlphabet)
		{
			m_cVariable = i;
			break;
		}
	}
	if (bCharInAlphabet)
	{
		m_cVariable = 231;
	}
}

bool CPattern::isStringInAlphabet(std::string _newString)
{
	bool bReturn = true;
	bool bCharInAlphabet = false;
	for (int i = 0; i < _newString.length(); ++i)
	{
		bCharInAlphabet = false;
		for (int j = 0; j < m_vAlphabet.size(); j++)
		{
			if (_newString.at(i) == m_vAlphabet.at(j).at(0))
			{
				bCharInAlphabet = true;
				break;
			}
		}
		if (!bCharInAlphabet)
		{
			bReturn = false;
			break;
		}
	}
	return bReturn;
}

bool CPattern::checkPattern(std::string _newString)
{
	if (!isStringInAlphabet(_newString))
	{
		return false;
	}

	if (m_sPattern.size() == 0 || m_sPattern.size() > _newString.length())
	{
		m_sPattern.clear();

		char tempChar[2];
		tempChar[1] = '\0';
		for (int i = 0; i < _newString.length(); i++)
		{
			tempChar[0] = _newString.at(i);
			std::string test(tempChar);
			m_sPattern.push_back(test);
		}

	}
	else if (m_sPattern.size() == _newString.length())
	{
		m_pChanges = std::vector<CPattern::SChanges>();
		char* tempChar = new char[2];
		tempChar[1] = '\0';

		for (int i = 0; i < m_sPattern.size(); ++i)
		{
			std::size_t foundNewString = m_sPattern.at(i).find(_newString.at(i));
			tempChar[0] = _newString.at(i);
			std::size_t foundVariable = m_sPattern.at(i).find(m_cVariable);
			if (foundNewString == std::string::npos && foundVariable == std::string::npos && m_sPattern.at(i).length() == 1)
			{
				bool hasfounded = false;
				CPattern::SChanges test;
				for (int j = 0; j < m_pChanges.size(); ++j)
				{
					test = m_pChanges.at(j);
					if (test.m_newWordChar.find(_newString.at(i)) != std::string::npos && test.m_PatternChar == m_sPattern.at(i))
					{
						hasfounded = true;
						break;
					}
				}

				if (hasfounded)
				{
					m_sPattern.at(i) = test.m_newVariable;
				}
				else
				{
					test.m_newWordChar = std::string(tempChar);
					test.m_PatternChar = m_sPattern.at(i);
					test.m_newVariable = getNewVariable();
					m_sPattern.at(i) = test.m_newVariable;
					m_pChanges.push_back(test);
				}
			}
			else if (foundVariable != std::string::npos &&  m_sPattern.at(i).length() > 1)
			{
				bool hasfounded = false;
				CPattern::SChanges test;
				for (int j = 0; j < m_pChanges.size(); ++j)
				{
					test = m_pChanges.at(j);
					if (test.m_PatternChar == m_sPattern.at(i) && test.m_newWordChar.find(_newString.at(i)) != std::string::npos)
					{
						hasfounded = true;
						break;
					}
				}

				if (hasfounded)
				{
					m_sPattern.at(i) = test.m_newVariable;
				}
				else
				{
					test.m_newWordChar = std::string(tempChar);
					test.m_PatternChar = m_sPattern.at(i);
					test.m_newVariable = getNewVariable();
					m_sPattern.at(i) = test.m_newVariable;
					m_pChanges.push_back(test);
				}
			}

		}
	}
	return true;
}

std::string CPattern::getNewVariable()
{
	int capacity = m_pChanges.size();
	std::vector<int> capacityArray;

	int temp = 0;
	std::vector<int>::iterator it;
	for (int i = 4; i > -1; --i)
	{
		temp = capacity % 10;
		capacity /= 10;
		it = capacityArray.begin();
		it = capacityArray.insert(it, temp);
		if (capacity == 0) break;
	}

	char* tempVariableChar = new char[capacityArray.size() + 2];
	tempVariableChar[0] = m_cVariable;
	for (int z = 0; z < capacityArray.size(); ++z)
	{
		tempVariableChar[z + 1] = capacityArray.at(z) + 48;
	}
	tempVariableChar[capacityArray.size() + 1] = '\0';
	std::string returnString(tempVariableChar);

	delete tempVariableChar;

	return returnString;
}