#pragma once
#include "Global.h"

/*

	- AssemblyLoader

	Load Disassembled assembly 

*/

namespace AssemblyLoader
{
	void Split(std::vector<std::string> &out, std::string orig, std::string delimiter)
	{
		size_t pos = 0;
		std::string token;
		while ((pos = orig.find(delimiter)) != std::string::npos) {
			token = orig.substr(0, pos);
			out.push_back(token);
			orig.erase(0, pos + delimiter.length());
		}
		out.push_back(orig);
	}

	void ParserLine(std::string sLine, LineData& pLine)
	{
		if (sLine.find(".func_") != std::string::npos || sLine.find(".end") != std::string::npos)
		{
			return;
		}
		std::vector<std::string> Splited;
		Split(Splited, sLine, "  ");
		if (Splited.size() > 1)
		{
			pLine.m_iAddress = std::stoi(Splited[0], nullptr, 16);

			// find opcode
			std::vector<std::string> Splited1;
			Split(Splited1, Splited[1], " ");
			if (Splited1.size() > 1)
			{
				pLine.m_iOpcode = Global::m_mOpcodeStringMap[Splited1[0]];

				// find operation number
				std::vector<std::string> Splited2;
				Split(Splited2, Splited1[1], "(");
				if (Splited2.size() > 1)
				{
					pLine.m_iOperateDataAddress = std::stoi(Splited2[0], nullptr, 16);
					
					// Resolve Symbols
					std::vector<std::string> Splited3;
					Split(Splited3, Splited2[1], "[");
					if (Splited3.size() > 1 && pLine.m_iOperateDataAddress > 0)
					{
						std::string VariableName = Splited3[0];

						std::vector<std::string> Splited4;
						Split(Splited4, Splited2[1], ")");

						std::vector<std::string> Splited5;
						Split(Splited5, Splited4[0], "[");

						if (Splited5.size() > 2)
						{
							Global::m_pSymbols[pLine.m_iOperateDataAddress] = make_pair(VariableName, Splited5[1] + "]");
						}
						else
						{
							Global::m_pSymbols[pLine.m_iOperateDataAddress] = make_pair(VariableName, Splited5[1].replace(Splited5[1].find("]"), 1, ""));
						}
					}
				}
				else {
					if ((pLine.m_iOpcode == PUSH || pLine.m_iOpcode == JMP || pLine.m_iOpcode == JNE) && Splited1[1].find("0x") != std::string::npos)
					{
						pLine.m_iOperateDataAddress = std::stoi(Splited1[1], nullptr, 16);
					}
					else if(pLine.m_iOpcode == EXTERN)
					{
						std::vector<std::string> Splited_;
						Split(Splited_, Splited1[1], "\"");
						if (Splited_.size() > 2)
						{
							ExternInfo ext;
							ext.m_iAddress = pLine.m_iAddress;
							ext.m_sExternName = Splited_[1];

							// resolve args, return value from string
							std::string sMethod = Splited_[1];

							std::vector<std::string> MethodSplit;
							Split(MethodSplit, sMethod, "__");
							if (MethodSplit.size() > 2)
							{
								//					Class + FuncName
								ext.m_sExternName = MethodSplit[0] + MethodSplit[1];
								ext.m_sClassName = MethodSplit[0];
								ext.m_sFunctionName = MethodSplit[1];
								if (MethodSplit[1][0] == 'o' && MethodSplit[1][1] == 'p')
								{
									ext.m_bRequiredThisPtr = false;
									ext.m_bIsOperator = true;
								}

								for (int i = 2; i < MethodSplit.size(); i++)
								{
									if (i == MethodSplit.size() - 1)
									{
										ext.m_bHasReturnValue = (MethodSplit[i] != "SystemVoid" ? true : false);
										break;
									}
									ext.m_bHasArgument = true;
								}

								if (ext.m_bHasArgument)
								{
									std::vector<std::string> ArgsSplit;
									Split(ArgsSplit, MethodSplit[2], "_");
									ext.m_iArgCount = ArgsSplit.size();
								}
							}
							Global::m_vExterns.push_back(ext);
						}
					}
				}
			}
			else // split failed, means the opcode dont have shits(e.g. COPY, POP, NOP)
			{
				pLine.m_iOpcode = Global::m_mOpcodeStringMap[Splited[1]];
			}
		}
	}

	void InitializeDefines()
	{
		Global::m_mOpcodeStringMap["NOP"] = NOP;
		Global::m_mOpcodeStringMap["PUSH"] = PUSH;
		Global::m_mOpcodeStringMap["POP"] = POP;
		Global::m_mOpcodeStringMap["JNE"] = JNE;
		Global::m_mOpcodeStringMap["JMP"] = JMP;
		Global::m_mOpcodeStringMap["EXTERN"] = EXTERN;
		Global::m_mOpcodeStringMap["ANNOTATION"] = ANNOTATION;
		Global::m_mOpcodeStringMap["JUMP_INDIRECT"] = JUMP_INDIRECT;
		Global::m_mOpcodeStringMap["COPY"] = COPY;

		Global::m_mOpcodeSizeMap[NOP] = 4;
		Global::m_mOpcodeSizeMap[PUSH] = 8;
		Global::m_mOpcodeSizeMap[POP] = 4;
		Global::m_mOpcodeSizeMap[JNE] = 8;
		Global::m_mOpcodeSizeMap[JMP] = 8;
		Global::m_mOpcodeSizeMap[EXTERN] = 8;
		Global::m_mOpcodeSizeMap[ANNOTATION] = 8;
		Global::m_mOpcodeSizeMap[JUMP_INDIRECT] = 8;
		Global::m_mOpcodeSizeMap[COPY] = 4;

		Global::m_mOperator["op_Subtraction"] = SUB;
		Global::m_mOperator["op_Addition"] = ADD;
		Global::m_mOperator["op_LessThan"] = LESSTHAN;
		Global::m_mOperator["op_GreaterThan"] = GREATERTHAN;
		Global::m_mOperator["op_Multiplication"] = MULTIPLY;
		Global::m_mOperator["op_Division"] = DIVIDE;
		Global::m_mOperator["op_Equality"] = EQUAL;
		Global::m_mOperator["op_Inequality"] = INEQUAL;


	}

	void LoadConstants()
	{
		std::stringstream ss(Global::sConstants);

		std::vector<std::string> m_vLines;
		std::string m_sLine;
		while (std::getline(ss, m_sLine, '\n'))
		{
			m_vLines.push_back(m_sLine);
		}

		for (int i = 0; i < m_vLines.size(); i++)
		{
			m_sLine = m_vLines[i];

			// const is string
			if (m_sLine.find("\"") != std::string::npos)
			{
				std::vector<std::string> ConstSplit;
				Split(ConstSplit, m_sLine, "\"");
				if (ConstSplit.size() > 1)
				{
					int m_iAddress = std::stoi(ConstSplit[0], nullptr, 16);
					Global::m_pConstVariables[m_iAddress] = std::string("\"") + ConstSplit[1] + std::string("\"");
				}
			}
			else {
				std::vector<std::string> ConstSplit;
				Split(ConstSplit, m_sLine, " ");
				if (ConstSplit.size() > 1)
				{
					int m_iAddress = std::stoi(ConstSplit[0], nullptr, 16);
					Global::m_pConstVariables[m_iAddress] = ConstSplit[1];
				}
			}
		}
	}

	void ParseAssembly()
	{
		std::stringstream ss(Global::sDisassembled);

		std::vector<std::string> m_vLines;
		std::string m_sLine;
		while (std::getline(ss, m_sLine, '\n'))
		{
			m_vLines.push_back(m_sLine);
		}
		
		bool m_bIsFuncStart = false;
		bool m_bIsFuncEnd = false;

		Function m_pFunction;

		std::vector<LineData> m_vLineData;
		for (int i = 0; i < m_vLines.size(); i++)
		{
			m_sLine = m_vLines[i];

			LineData m_pLine;

			if (m_sLine.find(".func_") != std::string::npos)
			{
				m_bIsFuncStart = true;
				continue;
			}
			if (m_sLine.find(".end") != std::string::npos)
			{
				m_bIsFuncEnd = true;
				if (i != m_vLines.size() - 1)
				{
					continue;
				}
			}
			ParserLine(m_sLine, m_pLine);
			if (m_bIsFuncStart)
			{
				m_pFunction.iStartAddress = m_pLine.m_iAddress;
				m_bIsFuncStart = false;
			}
			if (m_bIsFuncEnd)
			{
				m_pFunction.iEndAddress = m_vLineData[m_vLineData.size() - 1].m_iAddress;
				m_bIsFuncEnd = false;

				m_pFunction.vBytes.clear();
				for (int i = m_pFunction.iStartAddress; i < m_pFunction.iEndAddress; i++)
				{
					unsigned char Byte = Global::vBytes[i];

					m_pFunction.vBytes.push_back(Byte);
				}

				Global::m_vFunctions.push_back(m_pFunction);
				continue;
			}
			m_vLineData.push_back(m_pLine);
			//sAddLog("ParserLine: %d %d %d", m_pLine.m_iAddress, m_pLine.m_iOpcode, m_pLine.m_iOperateDataAddress);
		}
		AddLog("Loaded Functions: %d", Global::m_vFunctions.size());
		AddLog("Loaded Symbols:");
		for (const auto& [key, value] : Global::m_pSymbols) {
			AddLog("Address: 0x%p | Symbol: %s", key, value.first.c_str());
		}
	}

	void Load(const char* sDisassembledPath, const char* sByteCodePath, const char* sConstantPath)
	{
		std::ifstream m_iDisassembled(sDisassembledPath);

		std::string m_sBuf((std::istreambuf_iterator<char>(m_iDisassembled)),
			(std::istreambuf_iterator<char>()));

		Global::sDisassembled = m_sBuf;

		m_iDisassembled.close();
		//---------------------------------

		std::ifstream m_iConstant(sConstantPath);

		std::string m_sConstant((std::istreambuf_iterator<char>(m_iConstant)),
			(std::istreambuf_iterator<char>()));

		Global::sConstants = m_sConstant;

		m_iConstant.close();

		//---------------------------------
		std::ifstream m_iByteCode(sByteCodePath, std::ios::binary);

		std::vector<unsigned char> m_vRaw((std::istreambuf_iterator<char>(m_iByteCode)), std::istreambuf_iterator<char>());

		int m_iByteCodeLength = m_vRaw.size();
		Global::vBytes.resize(m_iByteCodeLength);

		memcpy(&Global::vBytes[0], &m_vRaw[0], m_iByteCodeLength);

		m_iByteCode.close();

		InitializeDefines();
		LoadConstants();
		ParseAssembly();
	}




}