#include "../stdafx.hpp"
#include "Tokenizer.hpp"

#include "TokenizerTypes.hpp"

using namespace Shakara;

TokenizeError Tokenizer::Tokenize(
	std::istream&       stream,
	std::vector<Token>& tokens
)
{
	// If there's a problem with the stream
	// return out
	if (!stream.good())
		return TokenizeError::BAD_STREAM;

	std::string value   = "";
	char        current = '\0';
	char        last    = '\0';

	// Whether or not we came upon a quote
	// and are parsing a string
	bool parsingString = false;

	// Whether or not we came upon a backslash
	// if so, set this so that the following
	// character will be escaped
	bool escapeNext = false;

	// Read through each character in the
	// file stream
	while (stream.get(current))
	{
		// Handle any kind of whitespace by
		// trying to get a token type from
		// the value currently in place
		if (
			!parsingString &&
			(current == ' '    ||
			current == '\n'   ||
			current == '\r\n' ||
			current == '\t')
		)
		{
			// We have nothing to tokenize!
			// Move on to the next character.
			if (value.empty())
				continue;

			Token token;
			_SetTokenFromValue(&token, value);

			tokens.push_back(token);

			value.clear();
			
			last = current;

			continue;
		}

		// If we hit a backslash and we are parsing a string
		// and we aren't supposed to escape it, set the flag
		// and continue;
		if (
			current == '\\' &&
			parsingString &&
			!escapeNext
		)
		{
			escapeNext = true;

			continue;
		}

		// If we hit a quote and we aren't parsing
		// a string already, start parsing, otherwise
		// stop parsing a string and add it as a token
		if (current == '"' && !parsingString)
		{
			parsingString = true;

			continue;
		}
		else if (
			current == '"' &&
			parsingString  &&
			!escapeNext
		)
		{
			parsingString = false;

			Token token;
			token.type  = TokenType::STRING;
			token.value = value;

			tokens.push_back(token);

			value.clear();

			last = current;

			continue;
		}

		// Try and tokenize a single
		// single character token like
		// the beginning of a block
		if (_IsSingleCharacterToken(current) && !parsingString)
		{
			// If we don't have an empty value
			// push the last value as a token
			if (!value.empty())
			{
				Token token;
				_SetTokenFromValue(&token, value);

				tokens.push_back(token);

				value.clear();
			}

			// Now, push the single character token
			Token token;
			_SetTokenFromValue(&token, std::string(1, current));

			// Try and make a urary operator
			if (_MakeUrary(
				token,
				tokens[tokens.size() - 1],
				last
			))
				tokens.pop_back();

			tokens.push_back(token);

			last = current;

			continue;
		}

		last = current;

		value.push_back(current);

		if (escapeNext)
			escapeNext = false;
	}

	// If there is still a value to tokenize, do it
	if (!value.empty())
	{
		Token token;
		_SetTokenFromValue(&token, value);

		tokens.push_back(token);

		value.clear();
	}

	return TokenizeError::NONE;
}

void Tokenizer::_SetTokenFromValue(Token* token, const std::string& value)
{
	TokenType type = TokenType::UNKNOWN;

	_DetermineTokenTypeFromValue(&type, value);

	(*token).type  = type;
	(*token).value = value;
}

bool Tokenizer::_MakeUrary(
	Token& current,
	const Token& last,
	const char lastChar
)
{
	// WARNING: Ugly code ahead!
	if (
		last.type    == TokenType::EQUAL &&
		current.type == TokenType::EQUAL &&
		lastChar     == '='
	)
	{
		current.type  = TokenType::COMPARISON;
		current.value = "==";

		return true;
	}
	else if (
		last.type    == TokenType::PLUS &&
		current.type == TokenType::PLUS &&
		lastChar     == '+'
	)
	{
		current.type  = TokenType::INCREMENT;
		current.value = "++";

		return true;
	}
	else if (
		last.type    == TokenType::MINUS &&
		current.type == TokenType::MINUS &&
		lastChar     == '-'
	)
	{
		current.type  = TokenType::DECREMENT;
		current.value = "--";

		return true;
	}
	else if (
		last.type    == TokenType::PLUS &&
		current.type == TokenType::EQUAL &&
		lastChar     == '+'
	)
	{
		current.type  = TokenType::PLUS_EQUAL;
		current.value = "+=";

		return true;
	}
	else if (
		last.type    == TokenType::MINUS &&
		current.type == TokenType::EQUAL &&
		lastChar     == '-'
	)
	{
		current.type  = TokenType::MINUS_EQUAL;
		current.value = "-=";

		return true;
	}
	else if (
		last.type    == TokenType::MULTIPLY &&
		current.type == TokenType::EQUAL &&
		lastChar     == '*'
	)
	{
		current.type  = TokenType::MULTIPLY_EQUAL;
		current.value = "*=";

		return true;
	}
	else if (
		last.type    == TokenType::DIVIDE &&
		current.type == TokenType::EQUAL &&
		lastChar     == '/'
	)
	{
		current.type = TokenType::DIVIDE_EQUAL;
		current.value = "/=";

		return true;
	}

	return false;
}

bool Tokenizer::_DetermineTokenTypeFromValue(TokenType* type, const std::string& value)
{
	// Grab the types of a one character
	// value
	if (value.size() == 1)
	{
		switch (value[0])
		{
		case '+':
			*type = TokenType::PLUS;
			return true;
		case '-':
			*type = TokenType::MINUS;
			return true;
		case '*':
			*type = TokenType::MULTIPLY;
			return true;
		case '/':
			*type = TokenType::DIVIDE;
			return true;
		case '=':
			*type = TokenType::EQUAL;
			return true;
		case '(':
			*type = TokenType::BEGIN_ARGS;
			return true;
		case ',':
			*type = TokenType::ARG_SEPERATOR;
			return true;
		case ')':
			*type = TokenType::END_ARGS;
			return true;
		case '{':
			*type = TokenType::BEGIN_BLOCK;
			return true;
		case '}':
			*type = TokenType::END_BLOCK;
			return true;
		default: break;
		}
	}

	if (value == "print")
	{
		*type = TokenType::PRINT;
		return true;
	}
	else if (value == "return")
	{
		*type = TokenType::RETURN;
		return true;
	}

	// Next, try and parse either an integer,
	// a decimal, or a identifier

	// True when there is any character in the
	// value that is a non-digit.
	//
	// Though, this is false with a decimal
	// point in between, as that still would be
	// "digits"
	bool containsDigits    = false;
	bool containsNonDigits = false;
	bool containsDecimal   = false;

	// Iterate through the value string 
	for (size_t index = 0; index < value.size(); index++)
	{
		char current = value[index];

		bool digit = isdigit(current);

		if (digit)
		{
			containsDigits = true;

			continue;
		}

		// If this is a decimal point
		// set the proper flag
		if (current == '.')
		{
			containsDecimal = true;

			continue;
		}

		// If this isn't a digit, and current
		// is not a a decimal point, set contains
		// non-digits accordingly
		if (!digit && current != '.')
		{
			containsNonDigits = true;

			continue;
		}
	}
	
	// If there are only non-digits, its
	// an identifier
	if (containsNonDigits && !containsDigits && !containsDecimal)
	{
		*type = TokenType::IDENTIFIER;

		return true;
	}

	// If there are only digits, its
	// an integer
	if (containsDigits && !containsNonDigits && !containsDecimal)
	{
		*type = TokenType::INTEGER;

		return true;
	}

	// If there are digits and a
	// decimal, its a decimal number
	if (containsDigits && containsDecimal && !containsNonDigits)
	{
		*type = TokenType::DECIMAL;

		return true;
	}

	*type = TokenType::UNKNOWN;

	return false;
}

bool Tokenizer::_IsSingleCharacterToken(char character)
{
	switch (character)
	{
	case '+':
	case '-':
	case '*':
	case '/':
	case '=':
	case '{':
	case '}':
	case '(':
	case ',':
	case ')':
		return true;
	}

	return false;
}