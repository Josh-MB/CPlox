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
		uint8_t constant = chunk.code[offset + 1];
		printf("%-16s %4d '", name.c_str(), constant);
		printValue(chunk.constants[constant]);
		printf("'\n");
		return offset + 2;
	}

	int disassembleInstruction(Chunk & chunk, int offset)
	{
		printf(" %04d ", offset);

		uint8_t instruction = chunk.code[offset];
		switch(instruction)
		{
		case OpCode::OP_CONSTANT:
			return constantInstruction("OP_CONSTANT", chunk, offset);
		case OpCode::OP_RETURN:
			return simpleInstruction("OP_RETURN", offset);
		default:
			printf("Unknown opcode %d\n", instruction);
			return offset + 1;
		}
		return 0;
	}
}