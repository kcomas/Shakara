#pragma once

namespace std
{
	class cout;
}

namespace Shakara
{
	namespace AST
	{
		class Node;

		class RootNode;

		class FunctionCall;

		class AssignmentNode;

		class BinaryOperation;
	}

	class Interpreter
	{
	public:
		Interpreter();

		Interpreter(std::ostream& output);

		~Interpreter();

		/**
		 * Pass in a AST root to start interpreting
		 * and actually executing code.
		 */
		void Execute(AST::RootNode* root);

	private:
		/**
		 * Output stream used for printing
		 */
		std::ostream& m_output;

		/**
		 * Map of strings to Nodes used for storing
		 * variables and their values globally
		 *
		 * Used by any Node that requires accessing
		 * an identifier
		 */
		std::map<const std::string, AST::Node*> m_globals;
		
		/**
		 * Take in an assignment node, and attempt to
		 * assign (and subsequently store) a variable
		 */
		void _ExecuteAssign(AST::AssignmentNode* assign);

		/**
		 * Take in a special print function call and
		 * print out the arguments
		 */
		void _ExecutePrint(AST::FunctionCall* print);

		/**
		 * Checks the type of a node and prints it
		 * accordingly.
		 */
		void _PrintTypedNode(AST::Node* node);

		/**
		 * Execute a binary operation and return a
		 * singular Node from it
		 */
		AST::Node* _ExecuteBinaryOperation(AST::BinaryOperation* operation);

		/**
		 * Get the value for an identifier within the
		 * globals map.
		 *
		 * Returns nullptr if none is found.
		 */
		AST::Node* _GetGlobal(const std::string& identifier);

	};
}