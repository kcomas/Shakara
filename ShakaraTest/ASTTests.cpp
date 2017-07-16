#include "stdafx.hpp"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace ShakaraTest
{
	namespace AST
	{
		TEST_CLASS(ASTTests)
		{
		public:
			TEST_METHOD(ASTBuildSingleAssignment)
			{
				// Create a test statement and insert
				// it into a stringstream
				std::string statement = "test = 1";

				std::stringstream stream(statement, std::ios::in);

				// Tokenize the stringstream
				std::vector<Shakara::Token> tokens;

				Shakara::Tokenizer tokenizer;
				tokenizer.Tokenize(stream, tokens);

				// Run the ASTBuilder to grab an AST
				Shakara::AST::RootNode   root;
				Shakara::AST::ASTBuilder builder;
				builder.Build(&root, tokens);

				// There should only be one child in
				// the root node, the assignment node
				Assert::AreEqual(
					static_cast<size_t>(1),
					static_cast<size_t>(root.Children())
				);

				// Now, recurse through and check each
				// type and make sure they are all
				// correct
				Shakara::AST::AssignmentNode* node = static_cast<Shakara::AST::AssignmentNode*>(root[0]);

				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::AST::NodeType::ASSIGN),
					static_cast<uint8_t>(node->Type())
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::AST::NodeType::IDENTIFIER),
					static_cast<uint8_t>(node->GetIdentifier()->Type())
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::AST::NodeType::INTEGER),
					static_cast<uint8_t>(node->GetAssignment()->Type())
				);
			}

			TEST_METHOD(ASTBuildBinaryOperationAssignment)
			{
				// Create a test statement and insert
				// it into a stringstream
				std::string statement = "test = 2 * 2";

				std::stringstream stream(statement, std::ios::in);

				// Tokenize the stringstream
				std::vector<Shakara::Token> tokens;

				Shakara::Tokenizer tokenizer;
				tokenizer.Tokenize(stream, tokens);

				// Run the ASTBuilder to grab an AST
				Shakara::AST::RootNode   root;
				Shakara::AST::ASTBuilder builder;
				builder.Build(&root, tokens);

				// There should only be one child in
				// the root node, the assignment node
				Assert::AreEqual(
					static_cast<size_t>(1),
					static_cast<size_t>(root.Children())
				);

				// Now, recurse through and check each
				// type and make sure they are all
				// correct
				Shakara::AST::AssignmentNode* node = static_cast<Shakara::AST::AssignmentNode*>(root[0]);

				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::AST::NodeType::ASSIGN),
					static_cast<uint8_t>(node->Type())
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::AST::NodeType::IDENTIFIER),
					static_cast<uint8_t>(node->GetIdentifier()->Type())
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::AST::NodeType::BINARY_OP),
					static_cast<uint8_t>(node->GetAssignment()->Type())
				);

				// Make sure that the correct values
				// are being loaded into the binary
				// operation
				Shakara::AST::BinaryOperation* operation = static_cast<Shakara::AST::BinaryOperation*>(node->GetAssignment());

				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::AST::NodeType::INTEGER),
					static_cast<uint8_t>(operation->GetLeftHand()->Type())
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::AST::NodeType::MULTIPLY),
					static_cast<uint8_t>(operation->Operation())
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::AST::NodeType::INTEGER),
					static_cast<uint8_t>(operation->GetRightHand()->Type())
				);
			}

			TEST_METHOD(ASTBuildMultiBinaryOperationAssignment)
			{
				// Create a test statement and insert
				// it into a stringstream
				std::string statement = "test = 2 * 2 / 2";

				std::stringstream stream(statement, std::ios::in);

				// Tokenize the stringstream
				std::vector<Shakara::Token> tokens;

				Shakara::Tokenizer tokenizer;
				tokenizer.Tokenize(stream, tokens);

				// Run the ASTBuilder to grab an AST
				Shakara::AST::RootNode   root;
				Shakara::AST::ASTBuilder builder;
				builder.Build(&root, tokens);

				// There should only be one child in
				// the root node, the assignment node
				Assert::AreEqual(
					static_cast<size_t>(1),
					static_cast<size_t>(root.Children())
				);

				// Now, recurse through and check each
				// type and make sure they are all
				// correct
				Shakara::AST::AssignmentNode* node = static_cast<Shakara::AST::AssignmentNode*>(root[0]);

				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::AST::NodeType::ASSIGN),
					static_cast<uint8_t>(node->Type())
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::AST::NodeType::IDENTIFIER),
					static_cast<uint8_t>(node->GetIdentifier()->Type())
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::AST::NodeType::BINARY_OP),
					static_cast<uint8_t>(node->GetAssignment()->Type())
				);

				// Make sure that the types are correct
				// including the nested binary operation
				Shakara::AST::BinaryOperation* operation = static_cast<Shakara::AST::BinaryOperation*>(node->GetAssignment());

				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::AST::NodeType::INTEGER),
					static_cast<uint8_t>(operation->GetLeftHand()->Type())
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::AST::NodeType::MULTIPLY),
					static_cast<uint8_t>(operation->Operation())
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::AST::NodeType::BINARY_OP),
					static_cast<uint8_t>(operation->GetRightHand()->Type())
				);

				// Make sure the nested is an integer divided
				// by another integer
				Shakara::AST::BinaryOperation* nested = static_cast<Shakara::AST::BinaryOperation*>(operation->GetRightHand());

				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::AST::NodeType::INTEGER),
					static_cast<uint8_t>(nested->GetLeftHand()->Type())
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::AST::NodeType::DIVIDE),
					static_cast<uint8_t>(nested->Operation())
				);
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::AST::NodeType::INTEGER),
					static_cast<uint8_t>(nested->GetRightHand()->Type())
				);
			}

			TEST_METHOD(ASTBuildMultiLineAssignment)
			{
				// Create a test statement and insert
				// it into a stringstream
				std::string code = R"(
					count    = 1
					argument = 2 * 2
					named    = 2 * 2 * 9 / 2
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

				// There should only be one child in
				// the root node, the assignment node
				Assert::AreEqual(
					static_cast<size_t>(3),
					static_cast<size_t>(root.Children())
				);

				// Check if the first child is an
				// assignment node
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::AST::NodeType::ASSIGN),
					static_cast<uint8_t>(root[0]->Type())
				);
			}

			TEST_METHOD(ASTBuildFunctionDeclaration)
			{
				// Create a test statement and insert
				// it into a stringstream
				std::string code = R"(
					count    = 1
					argument = 2 * 2
					
					add_to_counter = (value, other)
					{
						count = count + value

						count = count + value + value
					}
					
					stuff = 1
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

				// There should only be one child in
				// the root node, the assignment node
				Assert::AreEqual(
					static_cast<size_t>(4),
					static_cast<size_t>(root.Children())
				);

				// Check if the first child is an
				// assignment node
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::AST::NodeType::ASSIGN),
					static_cast<uint8_t>(root[0]->Type())
				);

				// Next, check if the second child
				// is also an assignment node
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::AST::NodeType::ASSIGN),
					static_cast<uint8_t>(root[1]->Type())
				);

				// Now for the more nitty gritty one
				// check if the function declaration
				// actually is a function declaration
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::AST::NodeType::FUNCTION),
					static_cast<uint8_t>(root[2]->Type())
				);

				// Now, check the declaration more thoroughly
				Shakara::AST::FunctionDeclaration* declaration = static_cast<Shakara::AST::FunctionDeclaration*>(root[2]);
			
				// Begin by checking if we have the right amount
				// of arguments for the function
				Assert::AreEqual(
					static_cast<size_t>(2),
					static_cast<size_t>(declaration->Arguments().size())
				);

				// Now check for the body of the function, which
				// is a RootNode stuck in the node
				//
				// Should be two nodes
				Shakara::AST::RootNode* body = static_cast<Shakara::AST::RootNode*>(declaration->Body());
				
				Assert::AreEqual(
					static_cast<size_t>(2),
					static_cast<size_t>(body->Children())
				);

				// Finally, check if the statement below the function
				// declaration is intact and outside of the function
				Assert::AreEqual(
					static_cast<uint8_t>(Shakara::AST::NodeType::ASSIGN),
					static_cast<uint8_t>(root[3]->Type())
				);
			}

		};
	}
}