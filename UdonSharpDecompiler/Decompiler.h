#pragma once
#include "Global.h"

namespace Decompiler
{
	std::vector<int> m_vStack;

	OPCODE IsAnyJumpToHere(Function UFunction, int iAddress, LineData& pOutLineData)
	{
		bool m_bSkipNext = false;

		int m_iAddress = UFunction.iStartAddress;

		unsigned int* Code = (unsigned int*)&UFunction.vBytes[0];

		for (int i = 0; i < UFunction.vBytes.size() / 4; i++)
		{
			if (m_bSkipNext)
			{
				m_bSkipNext = false;
				continue;
			}

			int opcode = _byteswap_ulong(Code[i]);
			switch (opcode)
			{
			case COPY:
			case POP:
			{
				m_iAddress += 4;
			}
			break;
			case EXTERN:
			case ANNOTATION:
			case PUSH:
			{
				m_iAddress += 8;
				m_bSkipNext = true;
			}
			break;
			case JNE:
			{
				int Address = _byteswap_ulong(Code[i + 1]);

				if (Address == iAddress)
				{
					pOutLineData.m_iAddress = m_iAddress;
					pOutLineData.m_iOpcode = JNE;
					pOutLineData.m_iOperateDataAddress = Address;
					return JNE;
				}

				m_iAddress += 8;
				m_bSkipNext = true;
			}
			break;
			case JMP:
			{
				int Address = _byteswap_ulong(Code[i + 1]);
				
				if (Address == iAddress)
				{
					pOutLineData.m_iAddress = m_iAddress;
					pOutLineData.m_iOpcode = JMP;
					pOutLineData.m_iOperateDataAddress = Address;
					return JMP;
				}

				m_iAddress += 8;
				m_bSkipNext = true;
			}
			break;
			case JUMP_INDIRECT:
			{
				int Address = _byteswap_ulong(Code[i + 1]);
				
				// should be ignored

				m_iAddress += 8;
				m_bSkipNext = true;
			}
			break;
			}
		}
		return NOP;
	}

	int FindBreakStatement(Function UFunction, int iStartAddress, int iEndAddress, int& iJneOutAddr)
	{
		bool m_bSkipNext = false;

		int m_iAddress = iStartAddress;

		unsigned int* Code = (unsigned int*)&UFunction.vBytes[iStartAddress];

		for (int i = 0; i < (iEndAddress - iStartAddress) / 4; i++)
		{
			if (m_bSkipNext)
			{
				m_bSkipNext = false;
				continue;
			}

			int opcode = _byteswap_ulong(Code[i]);
			switch (opcode)
			{
			case COPY:
			case POP:
			{
				m_iAddress += 4;
			}
			break;
			case EXTERN:
			case ANNOTATION:
			case PUSH:
			{
				m_iAddress += 8;
				m_bSkipNext = true;
			}
			break;
			case JNE:
			{
				int Address = _byteswap_ulong(Code[i + 1]);

				if (Address > iEndAddress)
				{
					iJneOutAddr = Address;
					return m_iAddress;
				}

				m_iAddress += 8;
				m_bSkipNext = true;
			}
			break;
			case JMP:
			{
				int Address = _byteswap_ulong(Code[i + 1]);

				m_iAddress += 8;
				m_bSkipNext = true;
			}
			break;
			case JUMP_INDIRECT:
			{
				int Address = _byteswap_ulong(Code[i + 1]);

				// TODO

				m_iAddress += 8;
				m_bSkipNext = true;
			}
			break;
			}
		}
		return 0;
	}

	bool FindElse(Function UFunction, int iStartAddress, int iEndAddress, int& ElseEndAddress)
	{
		bool m_bSkipNext = false;

		int m_iAddress = iStartAddress;

		unsigned int* Code = (unsigned int*)&UFunction.vBytes[iStartAddress];

		for (int i = 0; i < (iEndAddress - iStartAddress) / 4; i++)
		{
			if (m_bSkipNext)
			{
				m_bSkipNext = false;
				continue;
			}

			int opcode = _byteswap_ulong(Code[i]);
			switch (opcode)
			{
			case COPY:
			case POP:
			{
				m_iAddress += 4;
			}
			break;
			case EXTERN:
			case ANNOTATION:
			case PUSH:
			case JNE:
			{
				m_iAddress += 8;
				m_bSkipNext = true;
			}
			break;
			case JMP:
			{
				int Address = _byteswap_ulong(Code[i + 1]);

				ElseEndAddress = Address;
				return true;

				m_iAddress += 8;
				m_bSkipNext = true;
			}
			break;
			case JUMP_INDIRECT:
			{
				int Address = _byteswap_ulong(Code[i + 1]);


				return true;
				
				m_iAddress += 8;
				m_bSkipNext = true;
			}
			break;
			}
		}
		return 0;
	}

	template<typename T>
	T GetLastVector(std::vector<T> &Vec)
	{
		if (Vec.size() == 0)
			return { 0 };
		return Vec[Vec.size() - 1];
	}

	void Decompiler(Function UFunction)
	{
		bool m_bSkipNext = false;

		int m_iAddress = UFunction.iStartAddress;

		unsigned int* Code = (unsigned int*)&UFunction.vBytes[0];

		Result::AddLog("void Func_%d()", UFunction.iStartAddress);
		Result::AddLog("{");

		bool m_iIndentation = false;

		std::vector<LineData> m_iLastOpcodeType;
		int m_iOperateAddress = 0;

		std::vector<bool> m_bFoundLoop;
		std::vector<ControlFlowLoopData> m_pControlFlowLoopData;

		std::vector<bool> m_bCurrentIfhasElse;
		std::vector<int> m_iElseEndAddress;


		for (int i = 0; i < UFunction.vBytes.size() / 4; i++)
		{
			if (m_bSkipNext)
			{
				m_bSkipNext = false;
				continue;
			}
			
			LineData m_pPreWalkData;
			switch (IsAnyJumpToHere(UFunction, m_iAddress, m_pPreWalkData))
			{
			case JMP:
				
				// there is jmp back at bottom to the current address
				if (m_iAddress < m_pPreWalkData.m_iAddress)
				{
					m_bFoundLoop.push_back(true);

					ControlFlowLoopData m_pTemp;
					m_pTemp.m_iStartAddress = m_iAddress;
					m_pTemp.m_iEndAddress = m_pPreWalkData.m_iAddress;

					// Pre-Walk tho opcodes for finding breakstatement, it gonna find always correct address since logic is like that
					m_pTemp.m_iBreakOpcodeAddress = FindBreakStatement(UFunction, m_iAddress, m_pPreWalkData.m_iAddress, m_pTemp.m_iOutAddress);

					m_pControlFlowLoopData.push_back(m_pTemp);

					Result::AddLog("\twhile(true)");
					Result::AddLog("\t{");

					m_iIndentation = true;
				}
				else {
					if (GetLastVector(m_bCurrentIfhasElse) && m_iAddress == GetLastVector(m_iElseEndAddress))
					{
						Result::AddLog("\t}");
						m_bCurrentIfhasElse.pop_back();
					}
					else{
						Result::AddLog("LABEL_%d:", m_iAddress);
					}
				}

				break;
			case JNE:
				if (GetLastVector(m_bFoundLoop) && m_iAddress == GetLastVector(m_pControlFlowLoopData).m_iOutAddress)
				{
					Result::AddLog("\t}");
					m_bFoundLoop.pop_back();
				
					m_pControlFlowLoopData.pop_back();
				}
				else {
					if (GetLastVector(m_bCurrentIfhasElse))
					{
						Result::AddLog("\t}");
						Result::AddLog("\telse");
						Result::AddLog("\t{");
					}
					else {
						Result::AddLog("\t}");
					}
				}
				m_iIndentation = false;
				break;
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

				if (GetLastVector(m_bFoundLoop) && m_iAddress == GetLastVector(m_pControlFlowLoopData).m_iBreakOpcodeAddress)
				{
					Result::AddLog("\tif(%s)", m_sSymbol.c_str());
					Result::AddLog("\t\tbreak;");
				}
				else {
					int m_iElseAddrTemp = 0;
					bool m_bTempCurrentIfhasElse = FindElse(UFunction, m_iAddress - UFunction.iStartAddress, Address - UFunction.iStartAddress, m_iElseAddrTemp);

					m_bCurrentIfhasElse.push_back(m_bTempCurrentIfhasElse);
					m_iElseEndAddress.push_back(m_iElseAddrTemp);

					Result::AddLog("\tif(%s)", m_sSymbol.c_str());
					Result::AddLog("\t{");
					m_iIndentation = true;
				}

				m_iAddress += 8;
				m_bSkipNext = true;
			}
			break;
			case JMP:
			{
				int Address = _byteswap_ulong(Code[i + 1]);

				if (!GetLastVector(m_bFoundLoop) && !GetLastVector(m_bCurrentIfhasElse))
				{
					Result::AddLog("\tgoto LABEL_%d;", Address);
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
					else if(bIsOperator)
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
		Result::AddLog("}");
	}



}