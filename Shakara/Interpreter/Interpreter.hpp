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

		class FunctionDeclaration;
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
		 *
		 * The final three arguments are used for
		 * executing function bodies, and thus can
		 * be ignored for the end-user.
		 */
		void Execute(
			AST::RootNode*                          root,
			bool                                    function=false,
			AST::Node**                             returned=nullptr,
			std::map<const std::string, AST::Node*> scope=std::map<const std::string, AST::Node*>()
		);

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
		 * assign (and subsequently store) a variable.
		 *
		 * The local and scope arguments are used for
		 * functions, as to insert variables into local
		 * scope, preferably.
		 */
		void _ExecuteAssign(
			AST::AssignmentNode*                    assign,
			bool                                    local,
			std::map<const std::string, AST::Node*>& scope
		);

		/**
		 * Take in a function declaration node and store
		 * it as a variable in global scope.
		 */
		void _ExecuteFunctionDeclaration(AST::FunctionDeclaration* declaration);

		/**
		 * Take in a function call node, and attempt to
		 * assign parameters and run the function.
		 */
		void _ExecuteFunction(
			AST::FunctionCall* call,
			std::map<const std::string, AST::Node*>& scope
		);

		/**
		 * Take in a special print function call and
		 * print out the arguments
		 *
		 * The scope argument is used for functions
		 * to grab variables defined within.
		 */
		void _ExecutePrint(
			AST::FunctionCall* print,
			std::map<const std::string, AST::Node*>& scope
		);

		/**
		 * Checks the type of a node and prints it
		 * accordingly.
		 */
		void _PrintTypedNode(AST::Node* node);

		/**
		 * Execute a binary operation and return a
		 * singular Node from it
		 */
		AST::Node* _ExecuteBinaryOperation(
			AST::BinaryOperation* operation,
			std::map<const std::string, AST::Node*>& scope
		);

		/**
		 * Get the value for an identifier within the
		 * globals map.
		 *
		 * The scope parameter is optional and used for
		 * passing function defined variables to grab
		 * from.
		 *
		 * Returns nullptr if none is found.
		 */
		AST::Node* _GetGlobal(
			const std::string& identifier,
			std::map<const std::string, AST::Node*> scope=std::map<const std::string, AST::Node*>()
		);

	};
}