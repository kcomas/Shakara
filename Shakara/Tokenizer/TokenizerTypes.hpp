#pragma once

namespace Shakara
{
	enum class TokenizeError : uint8_t
	{
		NONE       = 0x00,
		BAD_STREAM = 0x01
	};

	enum class TokenType : uint8_t
	{
		UNKNOWN    = 0x00,
		IDENTIFIER = 0x01,

		/**
		 * Arithmatic Tokens
		 */
		PLUS        = 0x02,
		MINUS       = 0x03,
		MULTIPLY    = 0x04,
		DIVIDE      = 0x05,
		EQUAL       = 0x06,
		BEGIN_ARGS  = 0x07,
		END_ARGS    = 0x08,
		BEGIN_BLOCK = 0x09,
		END_BLOCK   = 0x0A,

		/**
		 * Type Tokens
		 */
		INTEGER    = 0x0B,
		DECIMAL    = 0x0C
	};

	struct Token
	{
		TokenType   type   = TokenType::UNKNOWN;
		std::string value  = "";
	};

	static inline bool IsArithmeticType(const TokenType& type)
	{
		return (type == TokenType::PLUS)     ||
			   (type == TokenType::MINUS)    ||
			   (type == TokenType::MULTIPLY) ||
			   (type == TokenType::DIVIDE);
	}
}