#include "stdafx.hpp"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ShakaraTest
{
	namespace Interpreter
	{
		TEST_CLASS(InterpreterTests)
		{
		public:
			TEST_METHOD(InterpetPrintStatement)
			{
				// Create a test statement and insert
				// it into a stringstream
				std::string code = R"(
					print("Shakara ", 1)
				)";

				std::stringstream stream(code, std::ios::in);

				// Tokenize the stringstream
				std::vector<Shakara::Token> tokens;

				Shakara::Tokenizer tokenizer;
				tokenizer.Tokenize(stream, tokens);

				// Run the ASTBuilder to grab an AST
				Shakara::AST::RootNode   root;
				Shakara::AST::ASTBuilder builder;
				builder.Build(&root, tokens);

				std::stringstream output;

				Shakara::Interpreter interpreter(output);
				interpreter.Execute(&root);

				Assert::AreEqual(
					"Shakara 1",
					output.str().c_str()
				);
			}

			TEST_METHOD(InterpetAssignmentAndPrint)
			{
				// Create a test statement and insert
				// it into a stringstream
				std::string code = R"(
					name = "Shakara"

					print(name)

					name = "Lang"

					print(name)
				)";

				std::stringstream stream(code, std::ios::in);

				// Tokenize the stringstream
				std::vector<Shakara::Token> tokens;

				Shakara::Tokenizer tokenizer;
				tokenizer.Tokenize(stream, tokens);

				// Run the ASTBuilder to grab an AST
				Shakara::AST::RootNode   root;
				Shakara::AST::ASTBuilder builder;
				builder.Build(&root, tokens);

				std::stringstream output;

				Shakara::Interpreter interpreter(output);
				interpreter.Execute(&root);

				Assert::AreEqual(
					"ShakaraLang",
					output.str().c_str()
				);
			}

			TEST_METHOD(InterpetBinaryOperationsAndPrint)
			{
				// Create a test statement and insert
				// it into a stringstream
				std::string code = R"(
					count = 1 + 1

					print(count)

					count = count - 1

					print(count)

					count = count * 4

					print(count)

					count = count / 2

					print(count)

					print("Shakar" + "a")
				)";

				std::stringstream stream(code, std::ios::in);

				// Tokenize the stringstream
				std::vector<Shakara::Token> tokens;

				Shakara::Tokenizer tokenizer;
				tokenizer.Tokenize(stream, tokens);

				// Run the ASTBuilder to grab an AST
				Shakara::AST::RootNode   root;
				Shakara::AST::ASTBuilder builder;
				builder.Build(&root, tokens);

				std::stringstream output;

				Shakara::Interpreter interpreter(output);
				interpreter.Execute(&root);

				// Should be "2142Shakara"
				Assert::AreEqual(
					"2142Shakara",
					output.str().c_str()
				);
			}

		};
	}
}