#include "stdafx.hpp"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ShakaraTest
{		
	namespace Tokenizer
	{
		TEST_CLASS(TokenizerTests)
		{
		public:
			TEST_METHOD(TokenizeStatement)
			{
				std::string statement = "test = 1";

				std::stringstream stream(statement, std::ios::in);

				std::vector<Shakara::Token> tokens;

				Shakara::Tokenizer tokenizer;
				tokenizer.Tokenize(stream, tokens);

				// Make sure that we have three tokens
				// for this test statement
				Assert::AreEqual(static_cast<size_t>(3), tokens.size());

				// Next, make sure that each token is
				// the right type
				//
				// The order should be, identifier,
				// equals, integer
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::TokenType::IDENTIFIER),
					static_cast<uint8_t>(tokens[0].type),
					L"Incorrect type! Expected an IDENTIFIER type."
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::TokenType::EQUAL),
					static_cast<uint8_t>(tokens[1].type),
					L"Incorrect Type! Expected an EQUAL type!"
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::TokenType::INTEGER),
					static_cast<uint8_t>(tokens[2].type),
					L"Incorrect type! Expected an INTEGER type"
				);
			}

			TEST_METHOD(TokenizeMultipleStatements)
			{
				std::string statement = R"(
				count = 1
				flags=2
			)";

				std::stringstream stream(statement, std::ios::in);

				std::vector<Shakara::Token> tokens;

				Shakara::Tokenizer tokenizer;
				tokenizer.Tokenize(stream, tokens);

				// Make sure that we have three tokens
				// for this test statement
				Assert::AreEqual(static_cast<size_t>(6), tokens.size());

				// Next, make sure that each token is
				// the right type
				//
				// The order should be, identifier,
				// equals, integer
				//
				// And since there is two equal statements,
				// they should be identical
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::TokenType::IDENTIFIER),
					static_cast<uint8_t>(tokens[0].type),
					L"Incorrect type! Expected an IDENTIFIER type."
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::TokenType::EQUAL),
					static_cast<uint8_t>(tokens[1].type),
					L"Incorrect Type! Expected an EQUAL type!"
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::TokenType::INTEGER),
					static_cast<uint8_t>(tokens[2].type),
					L"Incorrect type! Expected an INTEGER type"
				);

				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::TokenType::IDENTIFIER),
					static_cast<uint8_t>(tokens[3].type),
					L"Incorrect type! Expected an IDENTIFIER type."
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::TokenType::EQUAL),
					static_cast<uint8_t>(tokens[4].type),
					L"Incorrect Type! Expected an EQUAL type!"
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::TokenType::INTEGER),
					static_cast<uint8_t>(tokens[5].type),
					L"Incorrect type! Expected an INTEGER type"
				);
			}

			TEST_METHOD(TokenizeFunction)
			{
				std::string code = R"(
					count = 1
				
					add_to_counter = (value, other)
					{
						count = count + value
					}
				)";

				std::stringstream stream(code, std::ios::in);

				std::vector<Shakara::Token> tokens;

				Shakara::Tokenizer tokenizer;
				tokenizer.Tokenize(stream, tokens);

				// Make sure that we have three tokens
				// for this test statement
				Assert::AreEqual(static_cast<size_t>(17), tokens.size());

				// This is going to be long, make sure that
				// each type is accounted for, including
				// the count definition above the function
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::TokenType::IDENTIFIER),
					static_cast<uint8_t>(tokens[0].type),
					L"Incorrect type! Expected IDENTIFIER!"
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::TokenType::EQUAL),
					static_cast<uint8_t>(tokens[1].type),
					L"Incorrect type! Expected EQUAL!"
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::TokenType::INTEGER),
					static_cast<uint8_t>(tokens[2].type),
					L"Incorrect type! Expected INTEGER!"
				);

				// Go on to checking the types for the
				// function
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::TokenType::IDENTIFIER),
					static_cast<uint8_t>(tokens[3].type),
					L"Incorrect type! Expected IDENTIFIER!"
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::TokenType::EQUAL),
					static_cast<uint8_t>(tokens[4].type),
					L"Incorrect type! Expected IDENTIFIER!"
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::TokenType::BEGIN_ARGS),
					static_cast<uint8_t>(tokens[5].type),
					L"Incorrect type! Expected BEGIN_ARGS!"
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::TokenType::IDENTIFIER),
					static_cast<uint8_t>(tokens[6].type),
					L"Incorrect type! Expected IDENTIFIER!"
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::TokenType::ARG_SEPERATOR),
					static_cast<uint8_t>(tokens[7].type),
					L"Incorrect type! Expected ARG_SEPERATOR!"
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::TokenType::IDENTIFIER),
					static_cast<uint8_t>(tokens[8].type),
					L"Incorrect type! Expected IDENTIFIER!"
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::TokenType::END_ARGS),
					static_cast<uint8_t>(tokens[9].type),
					L"Incorrect type! Expected END_ARGS!"
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::TokenType::BEGIN_BLOCK),
					static_cast<uint8_t>(tokens[10].type),
					L"Incorrect type! Expected BEGIN_BLOCK!"
				);

				// Check that the assignment with adding
				// to the count variable is valid
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::TokenType::IDENTIFIER),
					static_cast<uint8_t>(tokens[11].type),
					L"Incorrect type! Expected IDENTIFIER!"
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::TokenType::EQUAL),
					static_cast<uint8_t>(tokens[12].type),
					L"Incorrect type! Expected EQUAL!"
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::TokenType::IDENTIFIER),
					static_cast<uint8_t>(tokens[13].type),
					L"Incorrect type! Expected IDENTIFIER!"
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::TokenType::PLUS),
					static_cast<uint8_t>(tokens[14].type),
					L"Incorrect type! Expected PLUS!"
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::TokenType::IDENTIFIER),
					static_cast<uint8_t>(tokens[15].type),
					L"Incorrect type! Expected IDENTIFIER!"
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::TokenType::END_BLOCK),
					static_cast<uint8_t>(tokens[16].type),
					L"Incorrect type! Expected END_BLOCK!"
				);
			}
		};
	}
}