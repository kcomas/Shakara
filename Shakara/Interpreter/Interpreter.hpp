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

		class BooleanNode;

		class IfStatement;
	}

	/**
	 * A structure containing a map of variables to identifiers,
	 * as well as a parent scope.
	 *
	 * Allows for recursive scope searching, and dynamic scope
	 * creation.
	 */
	struct Scope
	{
		Scope*                                  parent = nullptr;
		std::map<const std::string, AST::Node*> variables;

		~Scope();

		/**
		 * Attempt to insert a node into the current scope.
		 *
		 * Will also check the parent's ability to update
		 * beforehand.
		 */
		void Insert(const std::string& identifier, AST::Node* node);

		/**
		 * Attempt to update a node upward.
		 *
		 * If no update is made, false is returned.
		 */
		bool Update(const std::string& identifier, AST::Node* node);

		AST::Node* Search(const std::string& identifier);

		AST::Node* operator[](const std::string& identifier)
		{
			return variables[identifier];
		}
	};

	class Interpreter
	{
	public:
		Interpreter();

		Interpreter(std::ostream& output);

		void ErrorHandler(std::function<void()> handler)
		{
			m_errorHandle = handler;
		}

		/**
		 * Pass in a AST root to start interpreting
		 * and actually executing code.
		 *
		 * The final three arguments are used for
		 * executing function bodies, and thus can
		 * be ignored for the end-user.
		 */
		void Execute(
			AST::RootNode* root,
			bool           function=false,
			AST::Node**    returned=nullptr,
			Scope*         scope=nullptr
		);

	private:
		/**
		 * Output stream used for printing
		 */
		std::ostream& m_output;

		/**
		 * Callback function to be used for
		 * error handling
		 */
		std::function<void()> m_errorHandle;

		/**
		 * The global scope for each assignment and definition.
		 */
		Scope m_globalScope;
		
		/**
		 * Take in an assignment node, and attempt to
		 * assign (and subsequently store) a variable.
		 *
		 * The local and scope arguments are used for
		 * functions, as to insert variables into local
		 * scope, preferably.
		 */
		void _ExecuteAssign(
			AST::AssignmentNode* assign,
			bool                 local,
			Scope&               scope
		);

		/**
		 * Take in an if statement node, evaluate its condition
		 * and subsequently, if true, run its body.
		 *
		 * This function takes in whether or not its run within
		 * a function, as well as a returned node and a scope.
		 *
		 * The returned node is only for within functions, and is
		 * passed back to the function if not null.
		 */
		void _ExecuteIfStatement(
			AST::IfStatement* statement,
			bool              function,
			AST::Node**       returned,
			Scope&            scope
		);

		/**
		 * Take in a function declaration node and store
		 * it as a variable in global scope.
		 */
		void _ExecuteFunctionDeclaration(AST::FunctionDeclaration* declaration);

		/**
		 * Take in a function call node, and attempt to
		 * assign parameters and run the function.
		 *
		 * Returns a Node that represents the return of
		 * the function, can be nullptr.
		 */
		AST::Node* _ExecuteFunction(
			AST::FunctionCall* call,
			Scope&             scope
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
			Scope&             scope
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
			Scope&                scope
		);

		/**
		 * Execute a binary operation that should only
		 * contain logical portions, such as != or ==.
		 *
		 * This is mainly used for if statements or
		 * similar statements.
		 */
		AST::BooleanNode* _ExecuteLogicalBinaryOperation(
			AST::BinaryOperation* operation,
			Scope&                scope
		);

	};
}