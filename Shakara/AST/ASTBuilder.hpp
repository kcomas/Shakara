#pragma once

namespace Shakara
{
	struct Token;

	namespace AST
	{
		class RootNode;

		class Node;

		class BinaryOperation;

		/**
		 * Simple AST implementation for Shakara.
		 */
		class ASTBuilder
		{
		public:
			/**
			 * Build an AST into the passed in root node,
			 * using the passed in vector of tokens.
			 *
			 * This is a recursive method, with each call
			 * to something like "_ParseAssignment" returning
			 * an index to skip to
			 *
			 * The "stopAtEndBlock" parameter is used for function
			 * parsing
			 */
			void Build(
				RootNode*           root,
				std::vector<Token>& tokens,
				size_t              index = 0
			);

		private:
			/**
			 * Function that checks what the types are
			 * of the current token to build a node.
			 *
			 * Can be reused in things such as functions
			 *
			 * Returns true if a node was created or false
			 * if one was not made.
			 */
			bool _BuildIndividualNode(
				RootNode*           root,
				std::vector<Token>& tokens,
				size_t              index,
				ptrdiff_t*          next
			);

			/**
			 * Attempt to parse an assignment
			 * to a variable, such as an integer
			 * assignment or otherwise
			 */
			void _ParseVariableAssignment(
				RootNode*           root,
				std::vector<Token>& tokens,
				size_t              index,
				ptrdiff_t*          next
			);

			/**
			 * Attempt to parse a function call
			 * into a corresponding node.
			 */
			void _ParseFunctionCall(
				RootNode*           root,
				std::vector<Token>& tokens,
				size_t              index,
				ptrdiff_t*          next
			);

			/**
			 * Parse an increment or decrement
			 * assignment to a variable.
			 */
			void _ParseVariableIncrementDecrement(
				RootNode*           root,
				std::vector<Token>& tokens,
				size_t              index,
				ptrdiff_t*          next
			);

			/**
			 * Parse an arithmetic assignment to a
			 * variable.
			 *
			 * By arithmetic, I mean operators like
			 * += or -=.
			 */
			void _ParseVariableArithmeticAssignment(
				RootNode*           root,
				std::vector<Token>& tokens,
				size_t              index,
				ptrdiff_t*          next
			);

			/**
			 * Attempt to parse a function definition
			 */
			void _ParseFunctionDefinition(
				RootNode*           root,
				std::vector<Token>& tokens,
				size_t              index,
				ptrdiff_t*          next
			);

			/**
			 * Parses a binary operation
			 * from the current index
			 * until an end is found
			 */
			void _ParseBinaryOperation(
				BinaryOperation*    operation,
				std::vector<Token>& tokens,
				size_t              index,
				ptrdiff_t*          next
			);

			inline void _CreateSingleNodeFromToken(
				Node**        node,
				const Token& token
			);

		};
	}
}