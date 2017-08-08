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

			TEST_METHOD(InterpetUnaryOperations)
			{
				// Create a test statement and insert
				// it into a stringstream
				std::string code = R"(
					count = 24

					count += 4

					print(count)

					count -= 2

					print(count)

					count *= 2

					print(count)

					count /= 2

					print(count)

					count++

					print(count)
					
					count--

					print(count)

					name = "Shakar"

					name += "a"

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

				// Should be "282652262726Shakara"
				Assert::AreEqual(
					"282652262726Shakara",
					output.str().c_str()
				);
			}

			TEST_METHOD(InterpretFunctionDefinitionAndCall)
			{
				// Create a test statement and insert
				// it into a stringstream
				std::string code = R"(
					count = 0

					count_up = (amount)
					{
						adder = amount - 1

						count += adder

						return count
					}

					print(count_up(1))
					print(count_up(4))
					print(count_up(6))
					print(count_up(count_up(2)))
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

				// Should be "03817"
				Assert::AreEqual(
					"03817",
					output.str().c_str()
				);
			}

			TEST_METHOD(InterpretFunctionReturn)
			{
				// Create a test statement and insert
				// it into a stringstream
				std::string code = R"(
					exclaim_name = (name)
					{
						return name + "!"
					}

					print(exclaim_name("Maxwell"))
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

				// Should be "Maxwell!"
				Assert::AreEqual(
					"Maxwell!",
					output.str().c_str()
				);
			}

			TEST_METHOD(InterpretBooleanPrint)
			{
				// Create a test statement and insert
				// it into a stringstream
				std::string code = R"(
					has_name = true
					name     = "Maxwell"

					print(has_name)
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

				// Should be "true"
				Assert::AreEqual(
					"true",
					output.str().c_str()
				);
			}

			TEST_METHOD(InterpretIfStatement)
			{
				// Create a test statement and insert
				// it into a stringstream
				std::string code = R"(
					namer = (name)
					{
						if (name == "Maxwell")
							return name + "!"
						
						return name
					}

					print(namer("Maxwell"))
					print(namer("Max"))
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

				// Should be "Maxwell!Max"
				Assert::AreEqual(
					"Maxwell!Max",
					output.str().c_str()
				);
			}

			TEST_METHOD(InterpretWhileStatement)
			{
				// Create a test statement and insert
				// it into a stringstream
				std::string code = R"(
					counter = 0

					while (counter != 10)
					{
						counter++

						print(counter)
					}
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

				// Should be "12345678910"
				Assert::AreEqual(
					"12345678910",
					output.str().c_str()
				);
			}

		};
	}
}