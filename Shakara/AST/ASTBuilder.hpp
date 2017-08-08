#pragma once

namespace Shakara
{
	struct Token;

	namespace AST
	{
		class RootNode;

		class Node;

		class FunctionCall;

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
			 * Attempt to parse a return statement to
			 * the AST.
			 *
			 * In the interpreter, it will be made sure
			 * that this isn't called outside of a function.
			 */
			void _ParseReturnStatement(
				RootNode*           root,
				std::vector<Token>& tokens,
				size_t              index,
				ptrdiff_t*          next
			);

			/**
			 * Attempt to parse an if statement to the
			 * AST.
			 */
			void _ParseIfStatement(
				RootNode*           root,
				std::vector<Token>& tokens,
				size_t              index,
				ptrdiff_t*          next
			);

			/**
			 * Attempt to parse a logical operation into
			 * the AST.
			 */
			Node* _ParseLogicalOperation(
				std::vector<Token>& tokens,
				size_t              index,
				ptrdiff_t*          next
			);

			/**
			 * Attempt to parse a function call
			 * into a corresponding node and insert
			 * into the root node.
			 */
			void _ParseFunctionCall(
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
				FunctionCall*       call,
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
			 * Checks if the token after index matches
			 * a BEGIN_ARGs, which then means it's a
			 * function call and should be made as such.
			 *
			 * Next up, if that fails, it will check if it
			 * is a binary operation, after which, it will
			 * check for a singular type, like a INTEGER or
			 * string.
			 *
			 * The optional ignore logic boolean is used when
			 * already parsing a logical expression, so that
			 * an infinite memory loop is not occuring
			 */
			Node* _GetPassableNode(
				std::vector<Token>& tokens,
				size_t              index,
				ptrdiff_t*          next,
				bool                ignoreLogic=false
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