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
		UNKNOWN                 = 0x00,
		IDENTIFIER              = 0x01,

		/**
		 * Arithmetic Tokens
		 */
		PLUS                    = 0x02,
		MINUS                   = 0x03,
		MULTIPLY                = 0x04,
		DIVIDE                  = 0x05,
		MODULUS                 = 0x06,
		EQUAL                   = 0x07,
		BEGIN_ARGS              = 0x08,
		ARG_SEPERATOR           = 0x09,
		END_ARGS                = 0x0A,
		BEGIN_BLOCK             = 0x0B,
		END_BLOCK               = 0x0C,

		/**
		 * Type Tokens
		 */
		INTEGER                 = 0x0D,
		DECIMAL                 = 0x0E,
		STRING                  = 0x0F,

		/**
		 * Unary Tokens
		 */
		INCREMENT               = 0x10,
		DECREMENT               = 0x11,
		PLUS_EQUAL              = 0x12,
		MINUS_EQUAL             = 0x13,
		MULTIPLY_EQUAL          = 0x14,
		DIVIDE_EQUAL            = 0x15,
		MODULUS_EQUAL           = 0x16,

		/**
		 * Special Tokens (print, etc)
		 */
		PRINT                   = 0x17,
		RETURN                  = 0x18,

		/**
		 * Boolean and Logical Tokens
		 */
		BOOLEAN                 = 0x19,
		AND                     = 0x1A,
		OR                      = 0x1B,
		NOT                     = 0x1C,
		EQUAL_COMPARISON        = 0x1D,
		NOTEQUAL_COMPARISON     = 0x1E,
		LESS_COMPARISON         = 0x1F,
		GREATER_COMPARISON      = 0x20,
		LESSEQUAL_COMPARISON    = 0x21,
		GREATEREQUAL_COMPARISON = 0x22,

		/**
		 * Statements
		 */
		IF_STATEMENT            = 0x23,
		WHILE_STATEMENT         = 0x24
	};

	struct Token
	{
		TokenType   type   = TokenType::UNKNOWN;
		std::string value  = "";
		uint32_t    line   = 1;
		uint32_t    column = 0;
	};

	static inline bool IsBinaryType(const TokenType& type)
	{
		return (type == TokenType::PLUS)                 ||
			   (type == TokenType::MINUS)                ||
			   (type == TokenType::MULTIPLY)             ||
			   (type == TokenType::DIVIDE)               ||
			   (type == TokenType::MODULUS)              ||
			   (type == TokenType::EQUAL_COMPARISON)     ||
			   (type == TokenType::NOTEQUAL_COMPARISON)  ||
			   (type == TokenType::LESS_COMPARISON)      ||
			   (type == TokenType::GREATER_COMPARISON)   ||
			   (type == TokenType::LESSEQUAL_COMPARISON) ||
			   (type == TokenType::GREATEREQUAL_COMPARISON);
	}

	static inline bool IsUnaryType(const TokenType& type)
	{
		return (type == TokenType::INCREMENT)            ||
			   (type == TokenType::DECREMENT)            ||
			   (type == TokenType::PLUS_EQUAL)           ||
			   (type == TokenType::MINUS_EQUAL)          ||
			   (type == TokenType::MULTIPLY_EQUAL)       ||
			   (type == TokenType::DIVIDE_EQUAL)         ||
			   (type == TokenType::EQUAL_COMPARISON)     ||
			   (type == TokenType::NOTEQUAL_COMPARISON)  ||
			   (type == TokenType::LESS_COMPARISON)      ||     
			   (type == TokenType::GREATER_COMPARISON)   ||
			   (type == TokenType::LESSEQUAL_COMPARISON) ||
			   (type == TokenType::GREATEREQUAL_COMPARISON);
	}
}