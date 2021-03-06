#pragma once

namespace Shakara
{
	enum class TokenizeError : uint8_t;

	enum class TokenType : uint8_t;

	struct Token;

	/**
	 * A simple Tokenizer implementation for
	 * the language.
	 */
	class Tokenizer
	{
	public:
		/**
		 * Simply iterates through each character
		 * in the passed in stream, generating
		 * tokens along the way.
		 *
		 * The tokens vector should be passed in,
		 * and once the function is over, will be
		 * filled with individual tokens from the
		 * file.
		 *
		 * Returns a TokenizeError enum for errors
		 * encountered in parsing.
		 */	
		TokenizeError Tokenize(
			std::istream&       stream,
			std::vector<Token>& tokens
		);
	
	private:
		/**
		 * Sets a Token struct up based on the value
		 * passed in.
		 *
		 * Takes in a pointer to allocated memory for
		 * one token.
		 */
		void _SetTokenFromValue(Token* token, const std::string& value);

		/**
		 * Tries to make a unary operator from the current
		 * token, the last token, and the last character
		 * moved through.
		 *
		 * Returns true if a unary was found and changed, 
		 * or false otherwise.
		 */
		bool _MakeUnary(
			Token& current,
			const Token& last,
			const char lastChar
		);

		/**
		 * Grab a token type from the current value.
		 *
		 * Takes in a pointer to allocated memory
		 * for one token.
		 *
		 * Returns true if a type is found.
		 */
		bool _DetermineTokenTypeFromValue(TokenType* type, const std::string& value);

		/**
		 * Whether a character is a single token,
		 * such as equal or minus.
		 */
		bool _IsSingleCharacterToken(char character);

	};
}