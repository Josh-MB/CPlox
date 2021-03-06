#include "../include/debug.hpp"
#include "../include/value.hpp"
#include <cstdio>

namespace cplox 
{
	void disassembleChunk(Chunk & chunk, std::string const & name)
	{
		printf("== %s ==\n", name.c_str());

		for(auto n = chunk.code.size(), i = decltype(n){0}; i < n;)
		{
			i = disassembleInstruction(chunk, i);
		}
	}

	static int simpleInstruction(std::string const& name, int offset) 
	{
		printf("%s\n", name.c_str());
		return offset + 1;
	}

	static int constantInstruction(std::string const& name, Chunk& chunk, int offset)
	{
		uint8_t const constantIdx = chunk.code[offset + 1];
		printf("%-16s %4d '", name.c_str(), constantIdx);
		printValue(chunk.constants[constantIdx]);
		printf("'\n");
		return offset + 2;
	}

	static int longConstantInstruction(std::string const& name, Chunk& chunk, int offset)
	{
		int const constantIdx = chunk.code[offset + 1] | (chunk.code[offset + 2] << 8) | (chunk.code[offset + 3] << 16);
		printf("%-16s %4d '", name.c_str(), constantIdx);
		printValue(chunk.constants[constantIdx]);
		printf("'\n");
		return offset + 4;
	}

	int disassembleInstruction(Chunk & chunk, int offset)
	{
		printf(" %04d ", offset);
		auto const lineInfo = getLine(chunk, offset);
		if(offset > 0 && lineInfo.second == false)
		{
			printf("   | ");
		}
		else
		{
			printf("%4d ", lineInfo.first);
		}



		uint8_t const instruction = chunk.code[offset];
		switch(instruction)
		{
		case OpCode::OP_CONSTANT:
			return constantInstruction("OP_CONSTANT", chunk, offset);
		case OpCode::OP_CONSTANT_LONG:
			return longConstantInstruction("OP_CONSTANT_LONG", chunk, offset);
		case OpCode::OP_NEGATE:
			return simpleInstruction("OP_NEGATE", offset);
		case OpCode::OP_ADD:
			return simpleInstruction("OP_ADD", offset);
		case OpCode::OP_SUBTRACT:
			return simpleInstruction("OP_SUBTRACT", offset);
		case OpCode::OP_MULTIPLY:
			return simpleInstruction("OP_MULTIPLY", offset);
		case OpCode::OP_DIVIDE:
			return simpleInstruction("OP_DIVIDE", offset);
		case OpCode::OP_RETURN:
			return simpleInstruction("OP_RETURN", offset);
		default:
			printf("Unknown opcode %d\n", instruction);
			return offset + 1;
		}
		return 0;
	}
}