#include "../include/scanner.hpp"
#include "../include/common.hpp"

#include <cstdio>
#include <string>

namespace cplox
{
	// Helpful functions
	static bool isAlpha(char c) {
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_');
	}

	static bool isDigit(char c) {
		return (c >= '0' && c <= '9');
	}


	Scanner::Scanner(const char * source) : start{ source }, current{ source }
	{
	}	

	bool Scanner::match(char expected) {
		if(isAtEnd()) return false;
		if(*current != expected) return false;

		++current;
		return true;
	}

	Token Scanner::makeToken(TokenType type) {
		Token token;
		token.type = type;
		token.start = start;
		token.length = (int)(current - start);
		token.line = line;

		return token;
	}

	/**
	 * Constructs an error token using a string literal
	 * Note that tokens don't allocate their own memory, so token.start
	 * will point to message. This requires message to be a compile time
	 * char literal. May look into preallocating a buffer for "last error"
	 * which will allow for non-compile time strings, without dynamic
	 * allocation
	 * \param message - Compile time char literal containing error message
	 * \return Token with type TokenType::ERROR
	 */
	Token Scanner::errorToken(const char* message) {
		Token token;
		token.type = TokenType::ERROR;
		token.start = message;
		token.length = (int)strlen(message);
		token.line = line;

		return token;
	}

	void Scanner::skipWhitespace() {
		for(;;) {
			char c = peek();
			switch(c) {
			case ' ':
			case '\r':
			case '\t':
				advance();
				break;
			case '\n':
				++line;
				advance();
				break;
			case '/':
				if(peekNext() == '/') {
					// Consume to the end of the line, but not the final '\n' character
					while(peek() != '\n' && !isAtEnd()) advance();
				}
				else {
					return;
				}
				break;
			default:
				return;
			}
		}
	}

	TokenType Scanner::checkKeyword(int offset, int length, 
									const char* rest, TokenType type) const {
		if(current - start == offset + length &&
		   memcmp(start + offset, rest, length) == 0) {
			return type;
		}

		return TokenType::IDENTIFIER;
	}

	TokenType Scanner::identifierType() const {
		switch(start[0]) {
		case 'a': return checkKeyword(1, 2, "nd", TokenType::AND);
		case 'c': return checkKeyword(1, 4, "lass", TokenType::CLASS);
		case 'e': return checkKeyword(1, 3, "lse", TokenType::ELSE);
		case 'f':
			if(current - start > 1) {
				switch(start[1]) {
				case 'a': return checkKeyword(2, 3, "lse", TokenType::FALSE);
				case 'o': return checkKeyword(2, 1, "r", TokenType::FOR);
				case 'u': return checkKeyword(2, 1, "n", TokenType::FUN);
				}
			}
			break;
		case 'i': return checkKeyword(1, 1, "f", TokenType::IF);
		case 'n': return checkKeyword(1, 2, "il", TokenType::NIL);
		case 'o': return checkKeyword(1, 1, "r", TokenType::OR);
		case 'p': return checkKeyword(1, 4, "rint", TokenType::PRINT);
		case 'r': return checkKeyword(1, 5, "eturn", TokenType::RETURN);
		case 's': return checkKeyword(1, 4, "uper", TokenType::SUPER);
		case 't':
			if(current - start > 1) {
				switch(start[1]) {
				case 'h': return checkKeyword(2, 2, "is", TokenType::THIS);
				case 'r': return checkKeyword(2, 2, "ue", TokenType::TRUE);
				}
			}
			break;
		case 'v': return checkKeyword(1, 2, "ar", TokenType::VAR);
		case 'w': return checkKeyword(1, 4, "hile", TokenType::WHILE);
		}
		return TokenType::IDENTIFIER;
	}

	Token Scanner::identifier() {
		while(isAlpha(peek()) || isDigit(peek())) advance();

		return makeToken(identifierType());
	}

	Token Scanner::number() {
		while(isDigit(peek())) advance();

		// Look for a fractional part. Cannot specify number as 1. like in C++
		if(peek() == '.' && isDigit(peekNext())) {
			// Consume the "." and the remaining digits
			advance();

			while(isDigit(peek())) advance();
		}

		return makeToken(TokenType::NUMBER);
	}

	Token Scanner::string() {
		while(peek() != '"' && !isAtEnd()) {
			if(peek() == '\n') ++line;
			advance();
		}

		if(isAtEnd()) return errorToken("Unterminated string.");

		// Consume closing '"'
		advance();
		return makeToken(TokenType::STRING);
	}

	Token Scanner::scanToken()
	{
		skipWhitespace();
		start = current;

		if(isAtEnd()) return makeToken(TokenType::END_OF_FILE);

		char c = advance();
		if(isAlpha(c)) return identifier();
		if(isDigit(c)) return number();

		switch(c) {
		case '(': return makeToken(TokenType::LEFT_PAREN);
		case ')': return makeToken(TokenType::RIGHT_PAREN);
		case '{': return makeToken(TokenType::LEFT_BRACE);
		case '}': return makeToken(TokenType::RIGHT_BRACE);
		case ';': return makeToken(TokenType::SEMICOLON);
		case ',': return makeToken(TokenType::COMMA);
		case '.': return makeToken(TokenType::DOT);
		case '-': return makeToken(TokenType::MINUS);
		case '+': return makeToken(TokenType::PLUS);
		case '/': return makeToken(TokenType::SLASH);
		case '*': return makeToken(TokenType::STAR);
		case '!':
			return makeToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
		case '=':
			return makeToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
		case '<':
			return makeToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
		case '>':
			return makeToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
		case '"': return string();
		}
		return errorToken("Unexpected character.");
	}
	
}