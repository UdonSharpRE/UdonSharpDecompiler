#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cstdarg>
#include <map>


enum OPCODE {
	NOP,
	PUSH,
	POP,
	JNE = 4, //JUMP_IF_FALSE( if not equal)
	JMP,
	EXTERN,
	ANNOTATION,
	JUMP_INDIRECT,
	COPY,
};

enum OPERATOR {
	SUB,
	ADD,
	LESSTHAN,
	GREATERTHAN,
	MULTIPLY,
	DIVIDE,
	EQUAL,
	INEQUAL,
};

struct LineData {
	int m_iAddress = 0;
	OPCODE m_iOpcode = NOP;
	int m_iOperateDataAddress = 0;
};

struct ControlFlowLoopData {
	int m_iStartAddress = 0;
	int m_iEndAddress = 0;
	int m_iBreakOpcodeAddress = 0;
	int m_iOutAddress = 0;
};

struct Function
{
	int iStartAddress = 0;
	int iEndAddress = 0;
	std::vector<unsigned char> vBytes;

	Function() { }

	Function(int start, int end, unsigned char* bytes, size_t length)
	{
		iStartAddress = start;
		iEndAddress = end;
		
		vBytes.resize(length);
		memcpy(&vBytes[0], bytes, length);
	}
};

struct ExternInfo
{
	int m_iAddress = 0;
	int m_iArgCount = 0;
	
	bool m_bHasReturnValue = false;

	// this was default 'true' but in the case not all functions need this pointer even tho its object oriented language
	bool m_bRequiredThisPtr = true;
	
	bool m_bHasArgument = false;

	// means can be translate to like op_LessThan(a,b) = a < b 
	bool m_bIsOperator = false;

	std::string m_sExternName;
	std::string m_sClassName;
	std::string m_sFunctionName;
};

struct LineInfo {
	int m_iAddress;
	int m_iLineOrder;
};


void AddLog(const char* sLog, ...)
{
	va_list va;
	va_start(va, sLog);

	std::string m_sLog(sLog);
	m_sLog.append("\n");

	vprintf(m_sLog.c_str(), va);

	va_end(va);
}

void AddLogNoNewLine(const char* sLog, ...)
{
	va_list va;
	va_start(va, sLog);

	std::string m_sLog(sLog);

	vprintf(m_sLog.c_str(), va);

	va_end(va);
}

//#define PRE_PRINT_OUTPUT

namespace Result
{
	int m_iIndentationTimes = 0;

	std::vector<std::string> m_sDecompiled;
	int iCount = 0;
	
	void AddLog(const char* sLog, ...)
	{
		va_list va;
		va_start(va, sLog);

		char buffer[1024];
		vsprintf(buffer, sLog, va);

		std::string s(buffer);

		s.append("\n");
		for (int i = 0; i < m_iIndentationTimes; i++)
		{
			s = std::string("  ") + s;
		}
		m_sDecompiled.push_back(s);
		iCount++;
#ifdef PRE_PRINT_OUTPUT

		printf(s.c_str());
		
#endif

		va_end(va);
	}

	void AddLogNoNewLine(const char* sLog, ...)
	{
		va_list va;
		va_start(va, sLog);

		char buffer[1024];
		vsprintf(buffer, sLog, va);

		m_sDecompiled.push_back(buffer);
		iCount++;
#ifdef PRE_PRINT_OUTPUT

		printf(buffer);

#endif

		va_end(va);
	}

	void PrintAll()
	{
		for (auto& i : m_sDecompiled)
		{
			printf("%s", i.c_str());
		}
	}
}

namespace Global
{
	std::vector<unsigned char> vBytes;
	std::string sDisassembled;
	std::string sConstants;

	//						variableName TypeName
	std::map<int, std::pair<std::string, std::string>> m_pSymbols;

	std::vector<Function> m_vFunctions;
	std::vector<ExternInfo> m_vExterns;
	std::map<int, std::string> m_pConstVariables;

	std::map<std::string, OPCODE> m_mOpcodeStringMap;
	std::map<OPCODE, int> m_mOpcodeSizeMap;
	std::map<std::string, OPERATOR> m_mOperator;
}

namespace Utils
{
	ExternInfo GetExtern(int m_iAddress, int m_iOpAddress)
	{
		for (auto& i : Global::m_vExterns)
		{
			if (i.m_iAddress == m_iAddress)
				return i;
		}
		ExternInfo ext;
		ext.m_iAddress = m_iAddress;
		ext.m_sExternName = std::string("func_") + std::to_string(m_iOpAddress);
		return ext;
	}

	std::string GetSymbolFromAddress(int m_iAddress)
	{
		std::string m_sSymbol = Global::m_pSymbols[m_iAddress].first;
		if (m_sSymbol.empty())
		{
			return std::string("var_") + std::to_string(m_iAddress);
		}
		return m_sSymbol;
	}

	std::string GetSymbolTypeFromAddress(int m_iAddress)
	{
		std::string m_sSymbol = Global::m_pSymbols[m_iAddress].second;
		if (m_sSymbol.empty())
		{
			return std::string("object");
		}
		return m_sSymbol;
	}

	std::string GetConstVariableFromAddress(int m_iAddress, std::string m_sDefault)
	{
		std::string m_sSymbol = Global::m_pConstVariables[m_iAddress];
		if (m_sSymbol.empty())
		{
			return m_sDefault;
		}
		return m_sSymbol;
	}

	bool ResolveOperator(ExternInfo ext, std::string& out)
	{
		if (!ext.m_bIsOperator)
		{
			return false;
		}
		bool m_bFoundOperator = true;
		switch (Global::m_mOperator[ext.m_sFunctionName])
		{
		case SUB:
			out = " - ";
			break;
		case ADD:
			out = " + ";
			break;
		case LESSTHAN:
			out = " < ";
			break;
		case GREATERTHAN:
			out = " > ";
			break;
		case MULTIPLY:
			out = " * ";
			break;
		case DIVIDE:
			out = " / ";
			break;
		case EQUAL:
			out = " == ";
			break;
		case INEQUAL:
			out = " != ";
			break;
		default:
			m_bFoundOperator = false;
			break;
		}
		return m_bFoundOperator;
	}

	bool InAnyFunctionScope(int iJmpAddress)
	{
		for (int i = 0; i < Global::m_vFunctions.size(); i++)
		{
			auto& pFunction = Global::m_vFunctions[i];
			if (iJmpAddress > pFunction.iStartAddress && iJmpAddress < pFunction.iEndAddress)
			{
				return true;
			}
		}

		return false;
	}
}
