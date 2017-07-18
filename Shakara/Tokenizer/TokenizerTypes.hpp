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
		UNKNOWN        = 0x00,
		IDENTIFIER     = 0x01,

		/**
		 * Arithmatic Tokens
		 */
		PLUS           = 0x02,
		MINUS          = 0x03,
		MULTIPLY       = 0x04,
		DIVIDE         = 0x05,
		EQUAL          = 0x06,
		BEGIN_ARGS     = 0x07,
		ARG_SEPERATOR  = 0x08,
		END_ARGS       = 0x09,
		BEGIN_BLOCK    = 0x0A,
		END_BLOCK      = 0x0B,

		/**
		 * Type Tokens
		 */
		INTEGER        = 0x0C,
		DECIMAL        = 0x0D,

		/**
		 * Urary Tokens
		 */
		INCREMENT      = 0x0E,
		DECREMENT      = 0x0F,
		PLUS_EQUAL     = 0x10,
		MINUS_EQUAL    = 0x11,
		MULTIPLY_EQUAL = 0x12,
		DIVIDE_EQUAL   = 0x13,
		COMPARISON     = 0x14,
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

	static inline bool IsUraryType(const TokenType& type)
	{
		return (type == TokenType::INCREMENT)      ||
			   (type == TokenType::DECREMENT)      ||
			   (type == TokenType::PLUS_EQUAL)     ||
			   (type == TokenType::MINUS_EQUAL)    ||
			   (type == TokenType::MULTIPLY_EQUAL) ||
			   (type == TokenType::DIVIDE_EQUAL)   ||
			   (type == TokenType::COMPARISON);
	}
}