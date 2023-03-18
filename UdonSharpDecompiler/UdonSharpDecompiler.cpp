#include <iostream>
#include <Windows.h>

#include "Includes.h"

int main(int argc, char** argv)
{

	//AssemblyLoader::Load(argv[0], argv[1]);
	
	//AssemblyLoader::Load("D:\\Game Logic", "D:\\bin_Game Logic", "D:\\const_Game Logic");
	AssemblyLoader::Load("D:\\Mirror", "D:\\bin_Mirror", "D:\\const_Mirror");

	AddLog("\n");
	
	// the array is the function which one you wanna decompile they are ordered same as disassembled output
	Decompiler::Decompiler(Global::m_vFunctions[0]);

	Result::PrintAll();

	return 0;
}
