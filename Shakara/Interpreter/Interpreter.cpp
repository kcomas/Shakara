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
		delete itr.second;

	m_globals.clear();
}

void Interpreter::Execute(RootNode* root)
{
	// Go through each node in the AST and
	// start executing
	for (size_t index = 0; index < root->Children(); index++)
	{
		Node* node = (*root)[index];

		if (node->Type() == NodeType::CALL)
		{
			FunctionCall* call = static_cast<FunctionCall*>(node);

			if (call->Flags() == CallFlags::PRINT)
				_ExecutePrint(call);

			// TODO: Implement regular function calls
			// for now, just the special `print` call
			// is implemented
		}
		else if (node->Type() == NodeType::ASSIGN)
		{
			AssignmentNode* assign = static_cast<AssignmentNode*>(node);

			_ExecuteAssign(assign);
		}
	}
}

void Interpreter::_ExecuteAssign(AST::AssignmentNode* assign)
{
	// Grab the identifier string for getting (or adding)
	// to the map
	const std::string identifier = static_cast<IdentifierNode*>(assign->GetIdentifier())->Value();

	// Try and see if the identifier exists in the globals
	// map, if so, remove the previous value and add a new
	// node
	//
	// Otherwise, insert a new entry
	auto find = m_globals.find(identifier);

	Node* value = nullptr;

	// Check if the assigned value is a singular
	// type, and if so, just set it to a copy
	// of that node
	if (assign->GetAssignment()->Type() == NodeType::INTEGER)
		value = new IntegerNode(*(static_cast<IntegerNode*>((assign->GetAssignment()))));
	else if (assign->GetAssignment()->Type() == NodeType::DECIMAL)
		value = new DecimalNode(*(static_cast<DecimalNode*>((assign->GetAssignment()))));
	else if (assign->GetAssignment()->Type() == NodeType::STRING)
		value = new StringNode(*(static_cast<StringNode*>((assign->GetAssignment()))));
	else if (assign->GetAssignment()->Type() == NodeType::BINARY_OP)
		value = _ExecuteBinaryOperation(static_cast<BinaryOperation*>(assign->GetAssignment()));

	if (find != m_globals.end())
	{
		delete find->second;

		m_globals[identifier] = value;
	}
	else
		m_globals.insert(std::make_pair(identifier, value));
}

void Interpreter::_ExecutePrint(AST::FunctionCall* print)
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

			Node* node = _GetGlobal(identifier->Value());

			_PrintTypedNode(node);
		}
		else if (argument->Type() == NodeType::BINARY_OP)
		{
			// Grab the temporary operation result
			Node* result = _ExecuteBinaryOperation(static_cast<BinaryOperation*>(argument));

			_PrintTypedNode(result);

			// Result isn't required anymore, delete
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
	}
}

AST::Node* Interpreter::_ExecuteBinaryOperation(AST::BinaryOperation* operation)
{
	// Try and grab left and right hand of the
	// operation
	Node* leftHand  = operation->GetLeftHand();
	Node* rightHand = operation->GetRightHand();
	Node* result    = nullptr;

	// Grab the global variable for the identifier
	// to be used for operating
	if (leftHand->Type() == NodeType::IDENTIFIER)
		leftHand = _GetGlobal(static_cast<IdentifierNode*>(leftHand)->Value());
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

		// TODO: Implement error handler for say, exiting the program
		// in the case of a fatal error

		return nullptr;
	}

	if (rightHand->Type() == NodeType::IDENTIFIER)
		rightHand = _GetGlobal(static_cast<IdentifierNode*>(rightHand)->Value());
	// Nested binary operations should be handled recursively
	else if (rightHand->Type() == NodeType::BINARY_OP)
		rightHand = _ExecuteBinaryOperation(static_cast<BinaryOperation*>(rightHand));
	// Do the same as with the left hand, check if it is not
	// an operatable type, and if so throw an error
	else if (
		rightHand->Type() != NodeType::INTEGER &&
		rightHand->Type() != NodeType::DECIMAL &&
		rightHand->Type() != NodeType::STRING
	)
	{
		std::cerr << "Interpreter Error! Unrecognized type in right hand of operation!" << std::endl;

		// TODO: Implement error handler for say, exiting the program
		// in the case of a fatal error

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
		std::cerr << "Left-hand type: " << _GetNodeTypeName(leftHand->Type());
		std::cerr << "Right-hand type: " << _GetNodeTypeName(rightHand->Type()) << std::endl;

		// TODO: Implement a error callback for say exiting the
		// program if an error occurs

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

			return nullptr;
		}

		break;
	}
	default:
	{
		std::cerr << "Interpreter Error! Unrecognized operation type!" << std::endl;
		std::cerr << "Operation type: " << _GetNodeTypeName(operation->Operation()) << std::endl;

		return nullptr;
	}
	}

	return nullptr;
}

AST::Node* Interpreter::_GetGlobal(const std::string& identifier)
{
	auto find = m_globals.find(identifier);

	if (find == m_globals.end())
	{
		std::cerr << "Interpreter Error! Undefined variable \"" << identifier << "\"" << std::endl;

		return nullptr;
	}

	return find->second;
}