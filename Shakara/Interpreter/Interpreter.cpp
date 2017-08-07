#include "../stdafx.hpp"
#include "Interpreter.hpp"

#include "../AST/ASTNode.hpp"
#include "../AST/ASTTypes.hpp"
#include "../AST/Nodes/ASTRootNode.hpp"
#include "../AST/Nodes/ASTFunctionCallNode.hpp"
#include "../AST/Nodes/ASTIntegerNode.hpp"
#include "../AST/Nodes/ASTDecimalNode.hpp"
#include "../AST/Nodes/ASTStringNode.hpp"
#include "../AST/Nodes/ASTAssignmentNode.hpp"
#include "../AST/Nodes/ASTIdentifierNode.hpp"
#include "../AST/Nodes/ASTBinaryOperation.hpp"
#include "../AST/Nodes/ASTFunctionDeclarationNode.hpp"
#include "../AST/Nodes/ASTReturnNode.hpp"

using namespace Shakara;
using namespace Shakara::AST;

Interpreter::Interpreter()
	:
	Interpreter(std::cout)
{
}

Interpreter::Interpreter(std::ostream& output)
	:
	m_output(output)
{
}

Interpreter::~Interpreter()
{
	for (auto itr : m_globals)
		if (itr.second->Type() != NodeType::FUNCTION)
			delete itr.second;

	m_globals.clear();
}

void Interpreter::Execute(
	RootNode*                          root,
	bool                               function,
	Node**                             returned,
	std::map<const std::string, Node*> scope
)
{
	// Go through each node in the AST and
	// start executing
	for (size_t index = 0; index < root->Children(); index++)
	{
		Node* node = (*root)[index];

		if (node->Type() == NodeType::CALL)
		{
			FunctionCall* call = static_cast<FunctionCall*>(node);

			// Execute a special print function call
			// if the flag is set, otherwise, just
			// run a function
			if (call->Flags() == CallFlags::PRINT)
				_ExecutePrint(call, scope);
			else
				_ExecuteFunction(call, scope);
		}
		else if (node->Type() == NodeType::ASSIGN)
		{
			AssignmentNode* assign = static_cast<AssignmentNode*>(node);

			_ExecuteAssign(
				assign,
				function,
				scope
			);
		}
		else if (node->Type() == NodeType::FUNCTION)
		{
			if (function)
			{
				std::cerr << "Interpreter Error! Cannot declare a function within another function!" << std::endl;

				if (m_errorHandle)
					m_errorHandle();

				continue;
			}

			FunctionDeclaration* declaration = static_cast<FunctionDeclaration*>(node);

			_ExecuteFunctionDeclaration(declaration);
		}
		else if (node->Type() == NodeType::RETURN)
		{
			if (!function)
			{
				std::cerr << "Interpreter Error! Cannot return outside of a function body!" << std::endl;

				if (m_errorHandle)
					m_errorHandle();

				continue;
			}

			// Grab the return node and its returned value
			ReturnNode* returnNode = static_cast<ReturnNode*>(node);
			Node*       returnable = returnNode->GetReturned();

			// Create a new node based on the returned type
			// or throw an error if the type isn't a supported
			// type
			if (returnable->Type() == NodeType::IDENTIFIER)
			{
				// TODO: Find a way to get this 
				// actually i could use the mark delete
				// flag as true for all of these, and then
				// do it that way
				*returned = _GetGlobal(static_cast<IdentifierNode*>(returnable)->Value(), scope)->Clone();
				(*returned)->MarkDelete(true);
			}
			else if (
				returnable->Type() == NodeType::INTEGER ||
				returnable->Type() == NodeType::DECIMAL ||
				returnable->Type() == NodeType::STRING
			)
			{
				*returned = returnable->Clone();
				(*returned)->MarkDelete(true);
			}
			else if (returnable->Type() == NodeType::BINARY_OP)
			{
				*returned = _ExecuteBinaryOperation(static_cast<BinaryOperation*>(returnable), scope);
				(*returned)->MarkDelete(true);
			}
			else if (returnable->Type() == NodeType::CALL)
			{
				*returned = _ExecuteFunction(static_cast<FunctionCall*>(returnable), scope);
				(*returned)->MarkDelete(false);
			}

			// Since this is a return statement within a
			// function, once we hit it, we can break out
			// of execution.
			break;
		}
	}
}

void Interpreter::_ExecuteAssign(
	AssignmentNode* assign,
	bool local,
	std::map<const std::string, Node*>& scope
)
{
	// Grab the identifier string for getting (or adding)
	// to the map
	const std::string identifier = static_cast<IdentifierNode*>(assign->GetIdentifier())->Value();

	// Try and see if the identifier exists in the globals
	// map, if so, remove the previous value and add a new
	// node
	//
	// Though, if local is true, try finding in the globals
	// first, and if its there, modify, otherwise, try adding
	// to the scope map
	auto find = m_globals.find(identifier);

	Node* value = nullptr;

	// Check if the assigned value is a singular
	// type, and if so, just set it to a copy
	// of that node
	//
	// A binary operation will have the value set
	// to the result of the operation
	//
	// Function declarations are stored as the same
	// node from the AST, thus, if deleting a global
	// node, check if it is not a function declaration first
	if (assign->GetAssignment()->Type() == NodeType::INTEGER)
		value = new IntegerNode(*(static_cast<IntegerNode*>((assign->GetAssignment()))));
	else if (assign->GetAssignment()->Type() == NodeType::DECIMAL)
		value = new DecimalNode(*(static_cast<DecimalNode*>((assign->GetAssignment()))));
	else if (assign->GetAssignment()->Type() == NodeType::STRING)
		value = new StringNode(*(static_cast<StringNode*>((assign->GetAssignment()))));
	else if (assign->GetAssignment()->Type() == NodeType::BINARY_OP)
		value = _ExecuteBinaryOperation(static_cast<BinaryOperation*>(assign->GetAssignment()), scope);
	else if (assign->GetAssignment()->Type() == NodeType::CALL)
		value = _ExecuteFunction(static_cast<FunctionCall*>(assign->GetAssignment()), scope);

	if (!value)
	{
		std::cerr << "Interpreter Error! Invalid type used in assignment!" << std::endl;

		if (assign->GetAssignment()->Type() == NodeType::CALL)
			std::cerr << "Tried to use a function's return value as an assignment, when the function did not return!" << std::endl;
		else
			std::cerr << "Type: " << _GetNodeTypeName(assign->GetAssignment()->Type()) << std::endl;
	
		if (m_errorHandle)
			m_errorHandle();
	}

	// TODO: Add an error if the value is nullptr still

	if (find != m_globals.end())
	{
		if (find->second->Type() != NodeType::FUNCTION)
			delete find->second;

		m_globals[identifier] = value;
	}
	else
	{
		// If we should prefer the local scope, then
		// we try and find the identifier inside of
		// the scope map, if not found, it is added
		// to, otherwise, it is modified.
		//
		// If we don't prefer local, add to the global
		// map
		if (local)
		{
			find = scope.find(identifier);

			if (find != scope.end())
			{
				if (find->second->Type() != NodeType::FUNCTION)
					delete find->second;

				scope[identifier] = value;
			}
			else
				scope.insert(std::make_pair(identifier, value));
		}
		else
			m_globals.insert(std::make_pair(identifier, value));
	}
}

void Interpreter::_ExecuteFunctionDeclaration(AST::FunctionDeclaration* declaration)
{
	// Grab the identifier string for getting (or adding)
	// to the map
	const std::string identifier = static_cast<IdentifierNode*>(declaration->Identifier())->Value();

	// Try and see if the identifier exists in the globals
	// map, if so, remove the previous value and add a new
	// node
	//
	// Though, if local is true, try finding in the globals
	// first, and if its there, modify, otherwise, try adding
	// to the scope map
	auto find = m_globals.find(identifier);

	if (find != m_globals.end())
	{
		if (find->second->Type() != NodeType::FUNCTION)
			delete find->second;

		m_globals[identifier] = declaration;
	}
	else
		m_globals.insert(std::make_pair(identifier, declaration));
}

Node* Interpreter::_ExecuteFunction(
	FunctionCall* call,
	std::map<const std::string, Node*>& scope
)
{
	// First, try and find the actual function
	// declaration in the global map
	FunctionDeclaration* declaration = static_cast<FunctionDeclaration*>(_GetGlobal(static_cast<IdentifierNode*>(call->Identifier())->Value()));

	if (declaration->Arguments().size() != call->Arguments().size())
	{
		std::cerr << "Interpreter Error! Mismatched argument sizes!" << std::endl;
		std::cerr << "Expected: " << declaration->Arguments().size() << "; Got: " << call->Arguments().size() << "!" << std::endl;
	
		if (m_errorHandle)
			m_errorHandle();

		return nullptr;
	}

	// Create a temporary map to be used for storing
	// function variables
	std::map<const std::string, Node*> functionVars;

	// Now iterate through each argument within the call
	// and the definition to create variables to add to
	// the global map.
	//
	// These variables will be pushed later.
	for (size_t index = 0; index < call->Arguments().size(); index++)
	{
		Node* signature = declaration->Arguments()[index];
		Node* argument  = call->Arguments()[index];

		// The signature should always be an identifier, no
		// exceptions
		if (signature->Type() != NodeType::IDENTIFIER)
		{
			std::cerr << "Interpreter Error! Arguments within a function signature cannot be anything but a IDENTIFIER." << std::endl;
			std::cerr << "Recieved type of: " << _GetNodeTypeName(signature->Type()) << std::endl;
		
			if (m_errorHandle)
				m_errorHandle();
		}

		// Grab the name of the function declared variable from
		// the signature
		const std::string identifier = static_cast<IdentifierNode*>(signature)->Value();

		// Check if the identifier exists before adding it, and if
		// so throw an error
		auto exists = m_globals.find(identifier);

		if (exists != m_globals.end())
		{
			std::cerr << "Interpreter Error! Identifier \"" << identifier << "\" already exists in global scope!" << std::endl;
		
			if (m_errorHandle)
				m_errorHandle();

			continue;
		}

		// If trying to print an identifier, search for it
		// and then if a node is found, print it out correctly
		if (argument->Type() == NodeType::IDENTIFIER)
		{
			IdentifierNode* referencedName = static_cast<IdentifierNode*>(argument);

			Node* node = _GetGlobal(referencedName->Value(), scope);

			functionVars.insert(std::make_pair(identifier, node));
		}
		else if (argument->Type() == NodeType::BINARY_OP)
		{
			// Grab the temporary operation result
			Node* result = _ExecuteBinaryOperation(static_cast<BinaryOperation*>(argument), scope);
			result->MarkDelete(true);

			functionVars.insert(std::make_pair(identifier, result));
		}
		else if (argument->Type() == NodeType::CALL)
		{
			// Grab the temporary return result
			Node* result = _ExecuteFunction(static_cast<FunctionCall*>(argument), scope);

			if (!result)
			{
				std::cerr << "Interpreter Error! Tried to use function's return value as an argument, when nothing was returned!" << std::endl;

				if (m_errorHandle)
					m_errorHandle();
			}

			functionVars.insert(std::make_pair(identifier, result));
		}
		else
			functionVars.insert(std::make_pair(identifier, argument));
	}

	// Now that we have all of the arguments, execute the function
	// with the current arguments
	Node* returnNode = nullptr;
	
	Execute(
		static_cast<RootNode*>(declaration->Body()),
		true,
		&returnNode,
		functionVars
	);

	for (auto itr : functionVars)
		if (itr.second->MarkedForDeletion())
			delete itr.second;

	return returnNode;
}

void Interpreter::_ExecutePrint(
	FunctionCall* print,
	std::map<const std::string, Node*>& scope
)
{
	// Be sure that this is a print call
	if (print->Flags() != CallFlags::PRINT)
		return;

	// Iterate through each argument and attempt
	// to print them to the provided print handle
	for (size_t index = 0; index < print->Arguments().size(); index++)
	{
		Node* argument = print->Arguments()[index];

		// If trying to print an identifier, search for it
		// and then if a node is found, print it out correctly
		if (argument->Type() == NodeType::IDENTIFIER)
		{
			IdentifierNode* identifier = static_cast<IdentifierNode*>(argument);

			Node* node = _GetGlobal(identifier->Value(), scope);

			_PrintTypedNode(node);
		}
		else if (argument->Type() == NodeType::BINARY_OP)
		{
			// Grab the temporary operation result
			Node* result = _ExecuteBinaryOperation(static_cast<BinaryOperation*>(argument), scope);

			_PrintTypedNode(result);

			// Result isn't required anymore, delete
			delete result;
		}
		else if (argument->Type() == NodeType::CALL)
		{
			// Grab the temporary operation result
			Node* result = _ExecuteFunction(static_cast<FunctionCall*>(argument), scope);

			if (!result)
			{
				std::cerr << "Interpreter Error! Tried to use function's return value as an argument, when nothing was returned!" << std::endl;

				if (m_errorHandle)
					m_errorHandle();
			}

			_PrintTypedNode(result);

			// Result isn't required anymore, delete
			if (result->MarkedForDeletion())
				delete result;
		}
		else
			_PrintTypedNode(argument);
	}
}

void Interpreter::_PrintTypedNode(AST::Node* node)
{
	switch (node->Type())
	{
	case NodeType::INTEGER:
	{
		IntegerNode* integer = static_cast<IntegerNode*>(node);

		m_output << integer->Value();

		break;
	}
	case NodeType::DECIMAL:
	{
		DecimalNode* decimal = static_cast<DecimalNode*>(node);

		m_output << decimal->Value();

		break;
	}
	case NodeType::STRING:
	{
		StringNode* string = static_cast<StringNode*>(node);

		m_output << string->Value();

		break;
	}
	default:
	{
		m_output << _GetNodeTypeName(node->Type());

		break;
	}
	}
}

AST::Node* Interpreter::_ExecuteBinaryOperation(
	AST::BinaryOperation* operation,
	std::map<const std::string, AST::Node*>& scope
)
{
	// Try and grab left and right hand of the
	// operation
	Node* leftHand  = operation->GetLeftHand();
	Node* rightHand = operation->GetRightHand();
	Node* result    = nullptr;

	// Grab the global variable for the identifier
	// to be used for operating
	if (leftHand->Type() == NodeType::IDENTIFIER)
		leftHand = _GetGlobal(static_cast<IdentifierNode*>(leftHand)->Value(), scope);
	// For now, check if the left hand type is not a
	// operatable type, such as a string or an
	// integer, and if not, throw an error
	else if (
		leftHand->Type() != NodeType::INTEGER &&
		leftHand->Type() != NodeType::DECIMAL &&
		leftHand->Type() != NodeType::STRING
	)
	{
		std::cerr << "Interpreter Error! Unrecognized type in left hand of operation!" << std::endl;

		if (m_errorHandle)
			m_errorHandle();

		return nullptr;
	}

	if (rightHand->Type() == NodeType::IDENTIFIER)
		rightHand = _GetGlobal(static_cast<IdentifierNode*>(rightHand)->Value(), scope);
	// Nested binary operations should be handled recursively
	else if (rightHand->Type() == NodeType::BINARY_OP)
		rightHand = _ExecuteBinaryOperation(static_cast<BinaryOperation*>(rightHand), scope);
	// Do the same as with the left hand, check if it is not
	// an operatable type, and if so throw an error
	else if (
		rightHand->Type() != NodeType::INTEGER &&
		rightHand->Type() != NodeType::DECIMAL &&
		rightHand->Type() != NodeType::STRING
	)
	{
		std::cerr << "Interpreter Error! Unrecognized type in right hand of operation!" << std::endl;

		if (m_errorHandle)
			m_errorHandle();

		return nullptr;
	}

	// Now, determine what type to use for the result
	// If either of the sides of the operation are a
	// decimal, use that, otherwise, use the other type
	//
	// If the type is a string and any other type is
	// attempted to be operated to it, throw an error
	if (
		(leftHand->Type()  == NodeType::DECIMAL  ||
		 rightHand->Type() == NodeType::DECIMAL) &&
		(leftHand->Type()  != NodeType::STRING   &&
		 rightHand->Type() != NodeType::STRING)
	)
	{
		result = new DecimalNode();
		result->Type(NodeType::DECIMAL);
	}
	else if (
		leftHand->Type() == NodeType::INTEGER &&
		rightHand->Type() == NodeType::INTEGER
	)
	{
		result = new IntegerNode();
		result->Type(NodeType::INTEGER);
	}
	else if (
		leftHand->Type() == NodeType::STRING &&
		rightHand->Type() == NodeType::STRING
	)
	{
		result = new StringNode();
		result->Type(NodeType::STRING);
	}
	else
	{
		std::cerr << "Interpreter Error! Mismatched type for operation." << std::endl;
		std::cerr << "Left-hand type: " << _GetNodeTypeName(leftHand->Type()) << ";";
		std::cerr << " Right-hand type: " << _GetNodeTypeName(rightHand->Type()) << std::endl;

		if (m_errorHandle)
			m_errorHandle();

		return nullptr;
	}

	// Now, finally, operate on the left and right
	// hand nodes
	//
	// This is a super long block! Beware!
	switch (operation->Operation())
	{
	case NodeType::ADD:
	{
		if (result->Type() == NodeType::DECIMAL)
		{
			float leftVal  = 0.0f;
			float rightVal = 0.0f;

			// Since the type can be an integer or a
			// decmial, check and convert for both sides
			if (leftHand->Type() == NodeType::INTEGER)
				leftVal = static_cast<float>(static_cast<IntegerNode*>(leftHand)->Value());
			else if (leftHand->Type() == NodeType::DECIMAL)
				leftVal = static_cast<float>(static_cast<DecimalNode*>(leftHand)->Value());

			if (rightHand->Type() == NodeType::INTEGER)
				rightVal = static_cast<float>(static_cast<IntegerNode*>(rightHand)->Value());
			else if (rightHand->Type() == NodeType::DECIMAL)
				rightVal = static_cast<float>(static_cast<DecimalNode*>(rightHand)->Value());

			static_cast<DecimalNode*>(result)->Value(false, leftVal + rightVal);
			
			return result;
		}
		else if (result->Type() == NodeType::INTEGER)
		{
			static_cast<IntegerNode*>(result)->Value(
				false,
				static_cast<IntegerNode*>(leftHand)->Value() + static_cast<IntegerNode*>(rightHand)->Value()
			);

			return result;
		}
		else if (result->Type() == NodeType::STRING)
		{
			static_cast<StringNode*>(result)->Value(
				static_cast<StringNode*>(leftHand)->Value() + static_cast<StringNode*>(rightHand)->Value()
			);

			return result;
		}

		break;
	}
	case NodeType::SUBTRACT:
	{
		if (result->Type() == NodeType::DECIMAL)
		{
			float leftVal  = 0.0f;
			float rightVal = 0.0f;

			// Since the type can be an integer or a
			// decmial, check and convert for both sides
			if (leftHand->Type() == NodeType::INTEGER)
				leftVal = static_cast<float>(static_cast<IntegerNode*>(leftHand)->Value());
			else if (leftHand->Type() == NodeType::DECIMAL)
				leftVal = static_cast<float>(static_cast<DecimalNode*>(leftHand)->Value());

			if (rightHand->Type() == NodeType::INTEGER)
				rightVal = static_cast<float>(static_cast<IntegerNode*>(rightHand)->Value());
			else if (rightHand->Type() == NodeType::DECIMAL)
				rightVal = static_cast<float>(static_cast<DecimalNode*>(rightHand)->Value());

			static_cast<DecimalNode*>(result)->Value(false, leftVal - rightVal);

			return result;
		}
		else if (result->Type() == NodeType::INTEGER)
		{
			static_cast<IntegerNode*>(result)->Value(
				false,
				static_cast<IntegerNode*>(leftHand)->Value() - static_cast<IntegerNode*>(rightHand)->Value()
			);

			return result;
		}
		else if (result->Type() == NodeType::STRING)
		{
			std::cerr << "Interpreter Error! Cannot subtract a string from a string!" << std::endl;

			if (m_errorHandle)
				m_errorHandle();

			return nullptr;
		}

		break;
	}
	case NodeType::MULTIPLY:
	{
		if (result->Type() == NodeType::DECIMAL)
		{
			float leftVal  = 0.0f;
			float rightVal = 0.0f;

			// Since the type can be an integer or a
			// decmial, check and convert for both sides
			if (leftHand->Type() == NodeType::INTEGER)
				leftVal = static_cast<float>(static_cast<IntegerNode*>(leftHand)->Value());
			else if (leftHand->Type() == NodeType::DECIMAL)
				leftVal = static_cast<float>(static_cast<DecimalNode*>(leftHand)->Value());

			if (rightHand->Type() == NodeType::INTEGER)
				rightVal = static_cast<float>(static_cast<IntegerNode*>(rightHand)->Value());
			else if (rightHand->Type() == NodeType::DECIMAL)
				rightVal = static_cast<float>(static_cast<DecimalNode*>(rightHand)->Value());

			static_cast<DecimalNode*>(result)->Value(false, leftVal * rightVal);

			return result;
		}
		else if (result->Type() == NodeType::INTEGER)
		{
			static_cast<IntegerNode*>(result)->Value(
				false,
				static_cast<IntegerNode*>(leftHand)->Value() * static_cast<IntegerNode*>(rightHand)->Value()
			);

			return result;
		}
		else if (result->Type() == NodeType::STRING)
		{
			std::cerr << "Interpreter Error! Cannot multply a string by a string!" << std::endl;

			if (m_errorHandle)
				m_errorHandle();

			return nullptr;
		}

		break;
	}
	case NodeType::DIVIDE:
	{
		if (result->Type() == NodeType::DECIMAL)
		{
			float leftVal  = 0.0f;
			float rightVal = 0.0f;

			// Since the type can be an integer or a
			// decmial, check and convert for both sides
			if (leftHand->Type() == NodeType::INTEGER)
				leftVal = static_cast<float>(static_cast<IntegerNode*>(leftHand)->Value());
			else if (leftHand->Type() == NodeType::DECIMAL)
				leftVal = static_cast<float>(static_cast<DecimalNode*>(leftHand)->Value());

			if (rightHand->Type() == NodeType::INTEGER)
				rightVal = static_cast<float>(static_cast<IntegerNode*>(rightHand)->Value());
			else if (rightHand->Type() == NodeType::DECIMAL)
				rightVal = static_cast<float>(static_cast<DecimalNode*>(rightHand)->Value());

			static_cast<DecimalNode*>(result)->Value(false, leftVal / rightVal);

			return result;
		}
		else if (result->Type() == NodeType::INTEGER)
		{
			static_cast<IntegerNode*>(result)->Value(
				false,
				static_cast<IntegerNode*>(leftHand)->Value() / static_cast<IntegerNode*>(rightHand)->Value()
			);

			return result;
		}
		else if (result->Type() == NodeType::STRING)
		{
			std::cerr << "Interpreter Error! Cannot divide a string by a string!" << std::endl;
	
			if (m_errorHandle)
				m_errorHandle();

			return nullptr;
		}

		break;
	}
	default:
	{
		std::cerr << "Interpreter Error! Unrecognized operation type!" << std::endl;
		std::cerr << "Operation type: " << _GetNodeTypeName(operation->Operation()) << std::endl;

		if (m_errorHandle)
			m_errorHandle();

		return nullptr;
	}
	}

	return nullptr;
}

AST::Node* Interpreter::_GetGlobal(
	const std::string& identifier,
	std::map<const std::string, AST::Node*> scope
)
{
	auto find = m_globals.find(identifier);

	if (find == m_globals.end())
	{
		find = scope.find(identifier);

		if (find == scope.end())
		{
			std::cerr << "Interpreter Error! Variable \"" << identifier << "\" not found in current scope!" << std::endl;

			if (m_errorHandle)
				m_errorHandle();

			return nullptr;
		}
		else
			return find->second;
	}
	else
		return find->second;

	return nullptr;
}