#pragma once
#include "Global.h"

namespace Decompiler
{
	std::vector<int> m_vStack;
	std::vector<int> m_vLabels;
	std::vector<LineInfo> m_vLineInfo;

	void MarkLine(int iAddress)
	{
		LineInfo pLine;
		pLine.m_iAddress = iAddress;
		pLine.m_iLineOrder = Result::iCount;
		m_vLineInfo.push_back(pLine);
	}

	void MarkLabel(int iAddress)
	{
		for (int b = 0; b < m_vLabels.size(); b++)
		{
			if (m_vLabels[b] == iAddress)
			{
				m_vLabels[b] = iAddress; // if its getting multiple address keep latest one
				return;
			}
		}
		m_vLabels.push_back(iAddress);
	}

	void MarkLabelInLine(int iLine, int iAddress)
	{
		Result::m_sDecompiled.insert(Result::m_sDecompiled.begin() + iLine, std::string("LABEL_") + std::to_string(iAddress) + std::string(":\n"));
		Result::iCount++;
	}

	template<typename T>
	T GetLastVector(std::vector<T> &Vec)
	{
		if (Vec.size() == 0)
			return { 0 };
		return Vec[Vec.size() - 1];
	}

	void DecompilerRange(Function UFunction, int iStartAddress, int iEndAddress, bool bStub = false)
	{
		bool m_bSkipNext = false;

		//iStartAddress += 8;
		int m_iAddress = iStartAddress;

		unsigned int* Code = (unsigned int*)&UFunction.vBytes[0];
		if (!bStub)
		{
			// there is always push xxxxx as start of function its pointless
			//iStartAddress += 8;
			// there is always push xxxxx copy  jmp xxx as end of function its pointless
			//iEndAddress -= 20;
			Result::AddLog("void Func_%X()", UFunction.iStartAddress + 8);
			Result::AddLog("{");
		}
		else {

		}

		bool m_iIndentation = false;

		std::vector<LineData> m_iLastOpcodeType;
		int m_iOperateAddress = 0;

		std::vector<bool> m_bFoundLoop;
		std::vector<ControlFlowLoopData> m_pControlFlowLoopData;

		for (int i = (bStub ? (iStartAddress - UFunction.iStartAddress) / 4 : 0); i <= (iEndAddress - UFunction.iStartAddress) / 4; i++)
		{
			if (m_bSkipNext)
			{
				m_bSkipNext = false;
				continue;
			}
			//debug
			//Result::AddLog("%X", m_iAddress);
			for (int b = 0; b < m_vLabels.size(); b++)
			{
				if (m_vLabels[b] == m_iAddress)
				{
					m_vLabels.erase(m_vLabels.begin() + b);
					Result::AddLog("LABEL_%d:", m_iAddress);
				}
			}
			/*

			if (m_iIndentation)
			{
				Result::AddLogNoNewLine("\t");
			}

			*/

			int opcode = _byteswap_ulong(Code[i]);
			switch (opcode)
			{
			case NOP:
				break;
			case PUSH:
			{
				m_vStack.push_back(_byteswap_ulong(Code[i + 1]));

				//Result::AddLog("PUSH %p", _byteswap_ulong(Code[i + 1]));
				m_iOperateAddress;

				m_iAddress += 8;
				m_bSkipNext = true;
			}
			break;
			case POP:
			{
				m_vStack.pop_back();
				Result::AddLog("\tPOP");

				m_iAddress += 4;
			}
			break;
			case JNE:
			{
				int Statement = m_vStack[m_vStack.size() - 1];
				m_vStack.pop_back();

				std::string m_sSymbol = Utils::GetSymbolFromAddress(Statement);

				int Address = _byteswap_ulong(Code[i + 1]);

				MarkLabel(Address);

				MarkLine(m_iAddress);
				Result::AddLog("\tif(%s)", m_sSymbol.c_str());
				Result::AddLog("\t{");
				
				//Result::m_iIndentationTimes++;
				DecompilerRange(UFunction, m_iAddress + 8, Address, true);
				//Result::m_iIndentationTimes--;
				
				Result::AddLog("\t}");
				Result::AddLog("\telse {");
				Result::AddLog("\t\tgoto LABEL_%d;", Address);
				Result::AddLog("\t}");
				//Result::AddLog("\treturn;");
				//Result::AddLog("\tLABEL_%d", Address);

				i += (Address - m_iAddress) / 4;
				m_iAddress = Address + 8;
				MarkLine(m_iAddress);
				//m_bSkipNext = true;
			}
			break;
			case JMP:
			{
				int Address = _byteswap_ulong(Code[i + 1]);
				
				MarkLine(m_iAddress);
				
				Result::AddLog("\tgoto LABEL_%d;", Address);
				
				MarkLabel(Address);
				if (Address < m_iAddress) // jumping to back??
				{
					for (auto& LineInfo : m_vLineInfo)
					{
						if (LineInfo.m_iAddress == Address)
						{
							MarkLabelInLine(LineInfo.m_iLineOrder, LineInfo.m_iAddress);
						}
					}
				}

				m_iAddress += 8;
				m_bSkipNext = true;
			}
			break;
			case EXTERN:
			{
				int Address = _byteswap_ulong(Code[i + 1]);

				ExternInfo m_pExternInfo = Utils::GetExtern(m_iAddress, Address);

				std::string sOperator;
				bool bIsOperator = Utils::ResolveOperator(m_pExternInfo, sOperator);;

				MarkLine(m_iAddress);

				std::string Method = m_pExternInfo.m_sExternName;
				if (m_pExternInfo.m_bRequiredThisPtr)
				{
					int ThisPtr = m_vStack[m_vStack.size() - 1 - m_pExternInfo.m_iArgCount - (m_pExternInfo.m_bHasReturnValue ? 1 : 0)];

					std::string m_sSymbol = Utils::GetSymbolFromAddress(ThisPtr);
					if (m_sSymbol.find("_const_") != std::string::npos)
					{
						m_sSymbol = Utils::GetConstVariableFromAddress(ThisPtr, m_sSymbol);
					}

					Method = m_sSymbol + "." + m_pExternInfo.m_sFunctionName;
				}

				if (m_pExternInfo.m_bHasReturnValue)
				{
					int ReturnValue = m_vStack[m_vStack.size() - 1];
					m_vStack.pop_back();

					std::string m_sSymbol = Utils::GetSymbolFromAddress(ReturnValue);

					if (m_pExternInfo.m_bRequiredThisPtr)
					{
						Result::AddLogNoNewLine("\t%s = %s(", m_sSymbol.c_str(), Method.c_str());
					}
					else if (bIsOperator)
					{
						Result::AddLogNoNewLine("\t%s = ", m_sSymbol.c_str());
					}
					else {
						Result::AddLogNoNewLine("\t%s = %s(", m_sSymbol.c_str(), Method.c_str());
					}
				}
				else {
					Result::AddLogNoNewLine("\t%s(", Method.c_str());
				}

				std::vector<std::string> m_vTempStackReverse;
				for (int i = 0; i < m_pExternInfo.m_iArgCount; i++)
				{
					int ReturnValue = m_vStack[m_vStack.size() - 1];
					m_vStack.pop_back();

					std::string m_sSymbol = Utils::GetSymbolFromAddress(ReturnValue);
					if (m_sSymbol.find("_const_") != std::string::npos)
					{
						m_sSymbol = Utils::GetConstVariableFromAddress(ReturnValue, m_sSymbol);
					}

					m_vTempStackReverse.push_back(m_sSymbol);
				}
				std::reverse(m_vTempStackReverse.begin(), m_vTempStackReverse.end());
				for (int i = 0; i < m_pExternInfo.m_iArgCount; i++)
				{
					std::string m_sSymbol = m_vTempStackReverse[i];

					Result::AddLogNoNewLine("%s", m_sSymbol.c_str());
					if (i != m_pExternInfo.m_iArgCount - 1)
					{
						if (bIsOperator)
						{
							Result::AddLogNoNewLine(sOperator.c_str());
						}
						else
						{
							Result::AddLogNoNewLine(", ");
						}
					}
				}


				if (!bIsOperator)
				{
					Result::AddLog(");");
				}
				else {
					Result::AddLog(";");
				}
				m_iAddress += 8;
				MarkLine(m_iAddress);
				m_bSkipNext = true;
			}
			break;
			case ANNOTATION:
			{

			}
			break;
			case JUMP_INDIRECT:
			{
				int Address = _byteswap_ulong(Code[i + 1]);
				Result::AddLog("\tgoto LABEL_%d;", Address);

				m_iAddress += 8;
				m_bSkipNext = true;
			}
			break;
			case COPY:
			{
				MarkLine(m_iAddress);

				int Dest = m_vStack[m_vStack.size() - 1];
				m_vStack.pop_back();

				int Source = m_vStack[m_vStack.size() - 1];
				m_vStack.pop_back();

				std::string m_sDest = Utils::GetSymbolFromAddress(Dest);
				std::string m_sSrc = Utils::GetSymbolFromAddress(Source);

				if (m_sDest.find("_const_") != std::string::npos)
				{
					m_sDest = Utils::GetConstVariableFromAddress(Dest, m_sDest);
				}
				if (m_sSrc.find("_const_") != std::string::npos)
				{
					m_sSrc = Utils::GetConstVariableFromAddress(Source, m_sSrc);
				}

				Result::AddLog("\t%s = %s;", m_sDest.c_str(), m_sSrc.c_str());

				m_iAddress += 4;
				MarkLine(m_iAddress);
			}
			break;
			}
			if (opcode != PUSH)
			{
				/*
				LineData Line;
				Line.m_iAddress = m_iAddress;
				Line.m_iOpcode = (OPCODE)opcode;
				if ()
				{
					Line.m_iOperateDataAddress = ;
				}

				m_iLastOpcodeType.push_back(opcode);
				*/
			}
			
		}
		if(!bStub)
			Result::AddLog("}");
	}

	void Decompiler(Function UFunction)
	{
		DecompilerRange(UFunction, UFunction.iStartAddress, UFunction.iStartAddress + UFunction.vBytes.size());
	}



}