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
			 */
			void Build(
				RootNode*           root,
				std::vector<Token>& tokens,
				size_t              index = 0
			);

		private:
			/**
			 * Attempt to parse any type of
			 * assignment, whether it be an
			 * assignment of a function or
			 * just another type
			 */
			void _ParseAssignment(
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