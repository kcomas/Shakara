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
#include "../AST/Nodes/ASTBooleanNode.hpp"
#include "../AST/Nodes/ASTIfStatementNode.hpp"
#include "../AST/Nodes/ASTWhileStatementNode.hpp"
#include "../AST/Nodes/ASTArrayNode.hpp"
#include "../AST/Nodes/ASTArrayElementIdentifier.hpp"

using namespace Shakara;
using namespace Shakara::AST;

Scope::~Scope()
{
	for (auto itr : variables)
		if (itr.second->Type() != NodeType::FUNCTION && itr.second->MarkedForDeletion())
			delete itr.second;

	parent = nullptr;
}

Node* Scope::Search(const std::string& identifier)
{
	auto find = variables.find(identifier);

	// If there is a variable found within
	if (find != variables.end())
		return find->second;
	// Otherwise, try to recursively search
	else
		if (parent)
			return parent->Search(identifier);

	return nullptr;
}

void Scope::Insert(const std::string& identifier, Node* node)
{
	bool updated = false;

	if (parent)
		updated = parent->Update(identifier, node);

	if (!updated)
	{
		auto find = variables.find(identifier);

		if (find == variables.end())
			variables.insert(std::make_pair(identifier, node));
		else
		{
			if (find->second->Type() != NodeType::FUNCTION)
				delete find->second;

			variables[identifier] = node;
		}
	}
}

bool Scope::Update(const std::string& identifier, Node* node)
{
	bool alreadyUpdated = false;

	if (parent)
		alreadyUpdated = parent->Update(identifier, node);

	if (!alreadyUpdated)
	{
		auto find = variables.find(identifier);

		if (find == variables.end())
			return false;
		else
		{
			if (find->second->Type() != NodeType::FUNCTION)
				delete find->second;

			variables[identifier] = node;

			return true;
		}
	}

	return false;
}

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

void Interpreter::Execute(
	RootNode* root,
	bool      function,
	Node**    returned,
	Scope*    scope
)
{
	Scope& currentScope = ((scope) ? *scope : m_globalScope);

	// If we are in global scope, create the command arguments
	// array
	if (!scope)
		_CreateCommandArgumentsArray();

	// Go through each node in the AST and
	// start executing
	for (size_t index = 0; index < root->Children(); index++)
	{
		Node* node = (*root)[index];

		if (node->Type() == NodeType::CALL)
		{
			FunctionCall* call = static_cast<FunctionCall*>(node);

			_ExecuteFunction(call, currentScope);
		}
		else if (node->Type() == NodeType::ASSIGN)
		{
			AssignmentNode* assign = static_cast<AssignmentNode*>(node);

			if (assign->GetAssignment()->Type() == NodeType::ARRAY)
				_ExecuteArrayDeclaration(
					assign,
					currentScope
				);
			else
				_ExecuteAssign(
					assign,
					currentScope
				);
		}
		else if (node->Type() == NodeType::IF_STATEMENT)
		{
			IfStatement* statement = static_cast<IfStatement*>(node);

			_ExecuteIfStatement(
				statement,
				function,
				returned,
				currentScope
			);

			if (function && *returned)
				break;
		}
		else if (node->Type() == NodeType::WHILE_STATEMENT)
		{
			WhileStatement* statement = static_cast<WhileStatement*>(node);

			_ExecuteWhileStatement(
				statement,
				function,
				returned,
				currentScope
			);

			if (function && *returned)
				break;
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
				*returned = currentScope.Search(static_cast<IdentifierNode*>(returnable)->Value())->Clone();
				(*returned)->MarkDelete(true);
			}
			else if (
				returnable->Type() == NodeType::INTEGER ||
				returnable->Type() == NodeType::DECIMAL ||
				returnable->Type() == NodeType::STRING  ||
				returnable->Type() == NodeType::BOOLEAN
			)
			{
				*returned = returnable->Clone();
				(*returned)->MarkDelete(true);
			}
			else if (returnable->Type() == NodeType::BINARY_OP)
			{
				*returned = _ExecuteBinaryOperation(static_cast<BinaryOperation*>(returnable), currentScope);
				(*returned)->MarkDelete(true);
			}
			else if (returnable->Type() == NodeType::LOGICAL_OP)
			{
				*returned = _ExecuteLogicalOperation(static_cast<BinaryOperation*>(returnable), currentScope);
				(*returned)->MarkDelete(true);
			}
			else if (returnable->Type() == NodeType::CALL)
			{
				*returned = _ExecuteFunction(static_cast<FunctionCall*>(returnable), currentScope);
				(*returned)->MarkDelete(true);
			}
			else if (returnable->Type() == NodeType::ARRAY)
				*returned = returnable->Clone();
			else if (returnable->Type() == NodeType::ARRAY_ELEMENT_IDENTIFIER)
			{
				*returned = _GetArrayElement(static_cast<ArrayElementIdentifierNode*>(returnable), currentScope)->Clone();
				(*returned)->MarkDelete(true);
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
	Scope&          scope
)
{
	std::string identifier = "";
	// Grab the identifier depending on whether
	// it is an array element or not
	//
	// If it isn't grab a string for the whole
	// ident, if it is, grab a string for the
	// array ident, and then go from there
	if (assign->GetIdentifier()->Type() == NodeType::IDENTIFIER)
		identifier = static_cast<IdentifierNode*>(assign->GetIdentifier())->Value();
	else if (assign->GetIdentifier()->Type() == NodeType::ARRAY_ELEMENT_IDENTIFIER)
		identifier = static_cast<ArrayElementIdentifierNode*>(assign->GetIdentifier())->ArrayIdentifier();

	// Try and see if the identifier exists in the globals
	// map, if so, remove the previous value and add a new
	// node
	//
	// Though, if local is true, try finding in the globals
	// first, and if its there, modify, otherwise, try adding
	// to the scope map
	//Node* find = scope.Search(identifier);

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
		value = static_cast<IntegerNode*>(assign->GetAssignment())->Clone();
	else if (assign->GetAssignment()->Type() == NodeType::DECIMAL)
		value = static_cast<DecimalNode*>(assign->GetAssignment())->Clone();
	else if (assign->GetAssignment()->Type() == NodeType::STRING)
		value = static_cast<StringNode*>(assign->GetAssignment())->Clone();
	else if (assign->GetAssignment()->Type() == NodeType::BOOLEAN)
		value = static_cast<BooleanNode*>(assign->GetAssignment())->Clone();
	else if (assign->GetAssignment()->Type() == NodeType::BINARY_OP)
		value = _ExecuteBinaryOperation(static_cast<BinaryOperation*>(assign->GetAssignment()), scope);
	else if (assign->GetAssignment()->Type() == NodeType::LOGICAL_OP)
		value = _ExecuteLogicalOperation(static_cast<BinaryOperation*>(assign->GetAssignment()), scope);
	else if (assign->GetAssignment()->Type() == NodeType::CALL)
		value = _ExecuteFunction(static_cast<FunctionCall*>(assign->GetAssignment()), scope);
	else if (assign->GetAssignment()->Type() == NodeType::ARRAY_ELEMENT_IDENTIFIER)
		value = _GetArrayElement(static_cast<ArrayElementIdentifierNode*>(assign->GetAssignment()), scope)->Clone();

	if (!value)
	{
		std::cerr << "Interpreter Error! Invalid type used in assignment!" << std::endl;

		if (assign->GetAssignment()->Type() == NodeType::CALL)
			std::cerr << "Tried to use a function's return value as an assignment, when the function did not return!" << std::endl;
		else
			std::cerr << "Type: " << GetNodeTypeName(assign->GetAssignment()->Type()) << std::endl;
	
		if (m_errorHandle)
			m_errorHandle();
	}

	// If this is just a plain identifier, just
	// insert into the scope, otherwise, try
	// and set an element in an array
	if (assign->GetIdentifier()->Type() == NodeType::IDENTIFIER)
		scope.Insert(identifier, value);
	else if (assign->GetIdentifier()->Type() == NodeType::ARRAY_ELEMENT_IDENTIFIER)
	{
		Node* arrayNode = scope.Search(identifier);

		if (!arrayNode)
		{
			std::cerr << "Interpreter Error! Undeclared identifier \"" << identifier << "\"!";

			if (m_errorHandle)
				m_errorHandle();
		}

		if (arrayNode->Type() != NodeType::ARRAY)
		{
			std::cerr << "Interpreter Error! Cannot assign to a array element with a non-array type!" << std::endl;
			std::cerr << "Actual type: " << GetNodeTypeName(arrayNode->Type()) << std::endl;
		}

		// Grab the array index
		// Now, evaluate the index used
		// for the identifier
		Node* index = static_cast<ArrayElementIdentifierNode*>(assign->GetIdentifier())->Index()->Clone();

		if (index->Type() == NodeType::IDENTIFIER)
			index = scope.Search(static_cast<IdentifierNode*>(index)->Value())->Clone();
		else if (index->Type() == NodeType::BINARY_OP)
			index = _ExecuteBinaryOperation(static_cast<BinaryOperation*>(index), scope);
		else if (index->Type() == NodeType::LOGICAL_OP)
			index = _ExecuteLogicalOperation(static_cast<BinaryOperation*>(index), scope);
		else if (index->Type() == NodeType::CALL)
			index = _ExecuteFunction(static_cast<FunctionCall*>(index), scope);
		else if (index->Type() == NodeType::ARRAY_ELEMENT_IDENTIFIER)
			index = _GetArrayElement(static_cast<ArrayElementIdentifierNode*>(index), scope)->Clone();

		if (!index)
		{
			std::cerr << "Interpreter Error! Undefined index for array element assignment!" << std::endl;

			if (m_errorHandle)
				m_errorHandle();
		}

		if (index->Type() != NodeType::INTEGER)
		{
			std::cerr << "Interpreter Error! Index for array element assignment must be an integer!" << std::endl;
			std::cerr << "Actual Type: " << GetNodeTypeName(index->Type());

			delete index;

			if (m_errorHandle)
				m_errorHandle();
		}

		// Now, grab the index as a size_t
		int32_t arrIndex = static_cast<IntegerNode*>(index)->Value();

		// The index node is no longer needed, delete
		delete index;

		// Check if the index would be out of bounds
		if (arrIndex < 0 || static_cast<size_t>(arrIndex) >= static_cast<ArrayNode*>(arrayNode)->Size())
		{
			std::cerr << "Interpreter Error! Array index out of bounds!" << std::endl;
			std::cerr << "Index: " << arrIndex << "; Size: " << static_cast<ArrayNode*>(arrayNode)->Size() << std::endl;

			if (m_errorHandle)
				m_errorHandle();
		}

		// Finally, set the node accordingly
		static_cast<ArrayNode*>(arrayNode)->Set(arrIndex, value);
	}
}

void Interpreter::_ExecuteIfStatement(
	IfStatement* statement,
	bool         function,
	Node**       returnNode,
	Scope&       scope
)
{
	// First, try and evaluate the condition
	Node* condition = statement->Condition();

	if (condition->Type() == NodeType::IDENTIFIER)
		condition = scope.Search(static_cast<IdentifierNode*>(condition)->Value());
	else if (condition->Type() == NodeType::CALL)
		condition = _ExecuteFunction(static_cast<FunctionCall*>(condition), scope);
	else if (condition->Type() == NodeType::BINARY_OP)
		condition = _ExecuteBinaryOperation(static_cast<BinaryOperation*>(condition), scope);
	else if (condition->Type() == NodeType::LOGICAL_OP)
		condition = _ExecuteLogicalOperation(static_cast<BinaryOperation*>(condition), scope);

	// Make sure that the condition is a boolean value
	// otherwise, you can't exactly "evaluate" the statement
	if (condition->Type() != NodeType::BOOLEAN)
	{
		std::cerr << "Interpreter Error! If statement's condition must be of a boolean return!" << std::endl;
		std::cerr << "Current condition type: " << GetNodeTypeName(condition->Type()) << std::endl;

		if (m_errorHandle)
			m_errorHandle();

		return;
	}

	// Check the condition and if it is true, execute
	if (static_cast<BooleanNode*>(condition)->Value())
	{
		// Create another scope for the if statement, as
		// to not muddy up the previous scope
		Scope ifScope  = { 0 };
		ifScope.parent = &scope;

		Execute(
			static_cast<RootNode*>(statement->Body()),
			function,
			returnNode,
			&ifScope
		);
	}
	// If the condition is not true but we have an
	// else if, try and execute a new if statement
	else if (statement->ElseIfCondition())
		_ExecuteIfStatement(
			statement->ElseIfCondition(),
			function,
			returnNode,
			scope
		);
	// Now we check if there is a else condition
	// and if there is, execute that instead
	else if (statement->ElseBlock())
	{
		// Create a new scope separate from the if
		// statement scope to execute the else
		Scope elseScope = { 0 };
		elseScope.parent = &scope;

		Execute(
			static_cast<RootNode*>(statement->ElseBlock()),
			function,
			returnNode,
			&elseScope
		);
	}
}

void Interpreter::_ExecuteWhileStatement(
	WhileStatement* statement,
	bool         function,
	Node**       returnNode,
	Scope&       scope
)
{
	// First, try and evaluate the condition
	Node* condition = statement->Condition();

	if (condition->Type() == NodeType::IDENTIFIER)
		condition = scope.Search(static_cast<IdentifierNode*>(condition)->Value());
	else if (condition->Type() == NodeType::CALL)
		condition = _ExecuteFunction(static_cast<FunctionCall*>(condition), scope);
	else if (condition->Type() == NodeType::BINARY_OP)
		condition = _ExecuteBinaryOperation(static_cast<BinaryOperation*>(condition), scope);
	else if (condition->Type() == NodeType::LOGICAL_OP)
		condition = _ExecuteLogicalOperation(static_cast<BinaryOperation*>(condition), scope);

	// Make sure that the condition is a boolean value
	// otherwise, you can't exactly "evaluate" the statement
	if (condition->Type() != NodeType::BOOLEAN)
	{
		std::cerr << "Interpreter Error! If statement's condition must be of a boolean return!" << std::endl;
		std::cerr << "Current condition type: " << GetNodeTypeName(condition->Type()) << std::endl;

		if (m_errorHandle)
			m_errorHandle();

		return;
	}

	// Create another scope for the while statement, as
	// to not muddy up the previous scope
	Scope whileScope  = { 0 };
	whileScope.parent = &scope;

	// Now, for the while loop, while the condition evaluates to
	// false, execute and then subsequently re-evaluate
	while (static_cast<BooleanNode*>(condition)->Value())
	{
		Execute(
			static_cast<RootNode*>(statement->Body()),
			function,
			returnNode,
			&whileScope
		);

		// Re-evaluate using the same code as used above
		// First, try and evaluate the condition
		condition = statement->Condition();

		if (condition->Type() == NodeType::IDENTIFIER)
			condition = scope.Search(static_cast<IdentifierNode*>(condition)->Value());
		else if (condition->Type() == NodeType::CALL)
			condition = _ExecuteFunction(static_cast<FunctionCall*>(condition), scope);
		else if (condition->Type() == NodeType::BINARY_OP)
			condition = _ExecuteBinaryOperation(static_cast<BinaryOperation*>(condition), scope);
		else if (condition->Type() == NodeType::LOGICAL_OP)
			condition = _ExecuteLogicalOperation(static_cast<BinaryOperation*>(condition), scope);

		// Make sure that the condition is a boolean value
		// otherwise, you can't exactly "evaluate" the statement
		if (condition->Type() != NodeType::BOOLEAN)
		{
			std::cerr << "Interpreter Error! If statement's condition must be of a boolean return!" << std::endl;
			std::cerr << "Current condition type: " << GetNodeTypeName(condition->Type()) << std::endl;

			if (m_errorHandle)
				m_errorHandle();

			return;
		}
	}
}

void Interpreter::_ExecuteFunctionDeclaration(FunctionDeclaration* declaration)
{
	// Grab the identifier string for getting (or adding)
	// to the map
	const std::string identifier = static_cast<IdentifierNode*>(declaration->Identifier())->Value();

	m_globalScope.Insert(identifier, declaration);
}

void Interpreter::_ExecuteArrayDeclaration(
	AST::AssignmentNode* arrayAssignment,
	Scope&               scope
)
{
	// Grab the identifier string for adding the
	// array to the scope
	const std::string identifier = static_cast<IdentifierNode*>(arrayAssignment->GetIdentifier())->Value();

	// Grab the ArrayNode to be taken from
	ArrayNode* arrayNode = static_cast<ArrayNode*>(arrayAssignment->GetAssignment());

	// Also, create a new array node to store
	// all of the evaluated portions of the
	// declaration
	ArrayNode* finalArray = new ArrayNode();
	finalArray->Type(NodeType::ARRAY);

	// First, check if the array is fixed
	// and if so, evaluate the capacity
	// node and set it for the final array
	if (arrayNode->Fixed())
	{
		Node* capacity = arrayNode->Capacity();

		if (capacity->Type() == NodeType::IDENTIFIER)
			capacity = scope.Search(static_cast<IdentifierNode*>(capacity)->Value())->Clone();
		else if (capacity->Type() == NodeType::ARRAY_ELEMENT_IDENTIFIER)
			capacity = _GetArrayElement(static_cast<ArrayElementIdentifierNode*>(capacity), scope)->Clone();
		else if (capacity->Type() == NodeType::BINARY_OP)
			capacity = _ExecuteBinaryOperation(static_cast<BinaryOperation*>(capacity), scope);
		else if (capacity->Type() == NodeType::LOGICAL_OP)
			capacity = _ExecuteLogicalOperation(static_cast<BinaryOperation*>(capacity), scope);
		else if (capacity->Type() == NodeType::CALL)
			capacity = _ExecuteFunction(static_cast<FunctionCall*>(capacity), scope);
	
		// Make sure that the capacity is an integer
		if (!capacity || capacity->Type() != NodeType::INTEGER)
		{
			std::cerr << "Interpreter Error! Fixed-array capacity must be an integer!" << std::endl;
			
			if (capacity)
				std::cerr << "Actual Type: " << GetNodeTypeName(capacity->Type()) << std::endl;
		
			delete finalArray;

			if (m_errorHandle)
				m_errorHandle();
		}

		finalArray->Fixed(true);
		finalArray->Capacity(capacity);
	}

	// Now, if there are elements, iterate through
	for (size_t index = 0; index < arrayNode->Size(); index++)
	{
		Node* element = (*arrayNode)[index];

		// Convert types if required
		// For an identifier, search
		// through the scope and convert
		if (element->Type() == NodeType::IDENTIFIER)
			element = scope.Search(static_cast<IdentifierNode*>(element)->Value())->Clone();
		// For any singluar type, clone
		// and store
		else if (
			element->Type() == NodeType::INTEGER ||
			element->Type() == NodeType::DECIMAL ||
			element->Type() == NodeType::STRING ||
			element->Type() == NodeType::BOOLEAN
		)
			element = element->Clone();
		else if (element->Type() == NodeType::BINARY_OP)
			element = _ExecuteBinaryOperation(static_cast<BinaryOperation*>(element), scope);
		else if (element->Type() == NodeType::LOGICAL_OP)
			element = _ExecuteLogicalOperation(static_cast<BinaryOperation*>(element), scope);
		else if (element->Type() == NodeType::CALL)
			element = _ExecuteFunction(static_cast<FunctionCall*>(element), scope);
		else if (element->Type() == NodeType::ARRAY_ELEMENT_IDENTIFIER)
			element = _GetArrayElement(static_cast<ArrayElementIdentifierNode*>(element), scope)->Clone();

		// Now we can push the element to the array
		finalArray->Insert(element);

		// Make sure that we are not going over the
		// capacity
		int32_t capacityVal = static_cast<IntegerNode*>(finalArray->Capacity())->Value();

		if (finalArray->Size() > static_cast<size_t>(capacityVal))
		{
			std::cerr << "Interpreter Error! Element count in array definition, \"" << identifier << "\" is over the fixed capacity!" << std::endl;
			std::cerr << "Element count: " << finalArray->Size() << "; Capacity: " << capacityVal << std::endl;

			delete finalArray;

			if (m_errorHandle)
				m_errorHandle();
		}
	}

	// Insert into the scope
	scope.Insert(identifier, finalArray);
}

Node* Interpreter::_ExecuteFunction(
	FunctionCall* call,
	Scope&        scope
)
{
	// If this is a print call, run it as such
	// and return a nullptr (print returns nothing)
	if (call->Flags() == CallFlags::PRINT)
	{
		_ExecutePrint(
			call,
			scope
		);

		return nullptr;
	}
	else if (call->Flags() == CallFlags::TYPE)
		return _ExecuteType(
			call,
			scope
		);
	else if (call->Flags() == CallFlags::AMOUNT)
		return _ExecuteAmount(
			call,
			scope
		);

	// First, try and find the actual function
	// declaration in the global map
	FunctionDeclaration* declaration = static_cast<FunctionDeclaration*>(scope.Search(static_cast<IdentifierNode*>(call->Identifier())->Value()));

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
	Scope functionScope  = { 0 };
	functionScope.parent = &m_globalScope;

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
			std::cerr << "Recieved type of: " << GetNodeTypeName(signature->Type()) << std::endl;
		
			if (m_errorHandle)
				m_errorHandle();
		}

		// Grab the name of the function declared variable from
		// the signature
		const std::string identifier = static_cast<IdentifierNode*>(signature)->Value();

		// Check if the identifier exists before adding it, and if
		// so throw an error
		Node* exists = m_globalScope.Search(identifier);

		if (exists)
		{
			std::cerr << "Interpreter Error! Identifier \"" << identifier << "\" already exists in global scope!" << std::endl;
		
			if (m_errorHandle)
				m_errorHandle();

			continue;
		}

		// Try and get a identifier from global scope
		//
		// We grab from global scope, despite having
		// a scope passed to us, as we don't want to
		// have a function that is called have access
		// to the callees variables, also that could
		// bring many more conflicts and headaches
		if (argument->Type() == NodeType::IDENTIFIER)
		{
			IdentifierNode* referencedName = static_cast<IdentifierNode*>(argument);

			Node* node = m_globalScope.Search(referencedName->Value());

			functionScope.Insert(identifier, node);
		}
		else if (argument->Type() == NodeType::BINARY_OP)
		{
			// Grab the temporary operation result
			Node* result = _ExecuteBinaryOperation(static_cast<BinaryOperation*>(argument), scope);
			result->MarkDelete(true);

			functionScope.Insert(identifier, result);
		}
		else if (argument->Type() == NodeType::LOGICAL_OP)
		{
			// Grab the temporary operation result
			Node* result = _ExecuteLogicalOperation(static_cast<BinaryOperation*>(argument), scope);
			result->MarkDelete(true);

			functionScope.Insert(identifier, result);
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

			functionScope.Insert(identifier, result);
		}
		else
			functionScope.Insert(identifier, argument);
	}

	// Now that we have all of the arguments, execute the function
	// with the current arguments
	Node* returnNode = nullptr;
	
	Execute(
		static_cast<RootNode*>(declaration->Body()),
		true,
		&returnNode,
		&functionScope
	);

	return returnNode;
}

void Interpreter::_ExecutePrint(
	FunctionCall* print,
	Scope&        scope
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

			Node* node = scope.Search(identifier->Value());

			_PrintTypedNode(node);
		}
		else if (argument->Type() == NodeType::BINARY_OP)
		{
			// Grab the temporary operation result
			Node* result = _ExecuteBinaryOperation(static_cast<BinaryOperation*>(argument), scope);

			_PrintTypedNode(result);

			// Result isn't required anymore, delete
			if (result->MarkedForDeletion())
				delete result;
		}
		else if (argument->Type() == NodeType::LOGICAL_OP)
		{
			// Grab the temporary operation result
			Node* result = _ExecuteLogicalOperation(static_cast<BinaryOperation*>(argument), scope);

			_PrintTypedNode(result);

			// Result isn't required anymore, delete
			if (result->MarkedForDeletion())
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
		else if (argument->Type() == NodeType::ARRAY_ELEMENT_IDENTIFIER)
		{
			// Grab the temporary operation result
			Node* result = _GetArrayElement(static_cast<ArrayElementIdentifierNode*>(argument), scope);

			if (!result)
			{
				std::cerr << "Interpreter Error! Array element does not exist!" << std::endl;

				if (m_errorHandle)
					m_errorHandle();
			}

			_PrintTypedNode(result);
		}
		else
			_PrintTypedNode(argument);
	}
}

void Interpreter::_PrintTypedNode(Node* node)
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
	case NodeType::BOOLEAN:
	{
		BooleanNode* boolean = static_cast<BooleanNode*>(node);

		m_output << std::boolalpha << boolean->Value();

		break;
	}
	default:
	{
		m_output << GetNodeTypeName(node->Type());

		break;
	}
	}
}

Node* Interpreter::_ExecuteType(
	FunctionCall* type,
	Scope&        scope
)
{
	// Be sure that this is a print call
	if (type->Flags() != CallFlags::TYPE)
		return nullptr;

	// Make sure that only one argument is
	// in the call, as you can only grab the
	// type of one node
	if (type->Arguments().size() != 1)
	{
		std::cerr << "Interpreter Error! The \"type\" call can only be used with one argument!" << std::endl;
		std::cerr << "Argument amount: " << type->Arguments().size() << std::endl;

		if (m_errorHandle)
			m_errorHandle();
	}

	// If the argument is of a type, such
	// as a call or a binary op, execute
	// it, and then return the string
	Node*    arg     = type->Arguments()[0];
	NodeType argType = type->Arguments()[0]->Type();

	if (argType == NodeType::IDENTIFIER)
	{
		Node* value = scope.Search(static_cast<IdentifierNode*>(arg)->Value());
		argType     = value->Type();
	}
	else if (argType == NodeType::BINARY_OP)
	{
		Node* value = _ExecuteBinaryOperation(static_cast<BinaryOperation*>(arg), scope);
		argType     = value->Type();

		delete value;
	}
	else if (argType == NodeType::LOGICAL_OP)
	{
		Node* value = _ExecuteLogicalOperation(static_cast<BinaryOperation*>(arg), scope);
		argType     = value->Type();

		delete value;
	}
	else if (argType == NodeType::CALL)
	{
		Node* value = _ExecuteFunction(static_cast<FunctionCall*>(arg), scope);
		argType     = value->Type();

		delete value;
	}
	else if (argType == NodeType::ARRAY_ELEMENT_IDENTIFIER)
	{
		Node* value = _GetArrayElement(static_cast<ArrayElementIdentifierNode*>(arg), scope);
		argType = value->Type();
	}

	// Grab the single argument, and grab
	// the string for it, then subsequently
	// print the StringNode.
	StringNode* stringRep = new StringNode();
	stringRep->Type(NodeType::STRING);

	stringRep->Value(GetNodeTypeName(argType));

	return stringRep;
}

Node* Interpreter::_ExecuteAmount(
	FunctionCall* amount,
	Scope&        scope
)
{
	// Be sure that this is a print call
	if (amount->Flags() != CallFlags::AMOUNT)
		return nullptr;

	// Make sure that only one argument is
	// in the call, as you can only grab the
	// type of one node
	if (amount->Arguments().size() != 1)
	{
		std::cerr << "Interpreter Error! The \"amount\" call can only be used with one argument!" << std::endl;
		std::cerr << "Argument amount: " << amount->Arguments().size() << std::endl;

		if (m_errorHandle)
			m_errorHandle();
	}

	// If the argument is of a type, such
	// as a call or a binary op, execute
	// it, and then return the string
	Node*     arg         = amount->Arguments()[0];
	NodeType  currentType = arg->Type();
	Node*     value       = arg;
	bool      deleteNode  = false;

	// First, grab the identifier value before
	// trying to evaluate amounts
	if (currentType == NodeType::IDENTIFIER)
	{
		value       = scope.Search(static_cast<IdentifierNode*>(arg)->Value());
		currentType = value->Type();
		deleteNode  = false;
	}
	
	// Now, move onto trying to evaluate a binary
	// operation or a function call, which must
	// result in an array or a string
	if (currentType == NodeType::BINARY_OP)
	{
		value       = _ExecuteBinaryOperation(static_cast<BinaryOperation*>(arg), scope);
		currentType = value->Type();
		deleteNode  = true;
	}
	else if (currentType == NodeType::CALL)
	{
		value       = _ExecuteFunction(static_cast<FunctionCall*>(arg), scope);
		currentType = value->Type();
		deleteNode  = true;
	}
	else if (currentType == NodeType::ARRAY_ELEMENT_IDENTIFIER)
	{
		value       = _GetArrayElement(static_cast<ArrayElementIdentifierNode*>(arg), scope);
		currentType = value->Type();
		deleteNode  = false;
	}

	// Now, check if the value is of a string
	// or an array, and if so, return the amount
	// node
	if (currentType == NodeType::ARRAY)
	{
		ArrayNode* arr = static_cast<ArrayNode*>(value);
		
		IntegerNode* amt = new IntegerNode();
		amt->Type(NodeType::INTEGER);
		amt->Value(false, arr->Size());

		return amt;
	}
	else if (currentType == NodeType::STRING)
	{
		StringNode* str = static_cast<StringNode*>(value);

		IntegerNode* amt = new IntegerNode();
		amt->Type(NodeType::INTEGER);
		amt->Value(false, str->Value().size());

		return amt;
	}
	else
	{
		std::cerr << "Interpreter Error! The element to grab the \"amount\" of must be an Array or String!" << std::endl;
		std::cerr << "Current type: " << GetNodeTypeName(currentType) << std::endl;

		if (deleteNode)
			delete value;

		if (m_errorHandle)
			m_errorHandle();
	}

	if (deleteNode)
		delete value;

	return nullptr;
}

Node* Interpreter::_ExecuteBinaryOperation(
	BinaryOperation* operation,
	Scope&                scope
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
		leftHand = scope.Search(static_cast<IdentifierNode*>(leftHand)->Value());
	else if (leftHand->Type() == NodeType::CALL)
		leftHand = _ExecuteFunction(static_cast<FunctionCall*>(leftHand), scope);
	else if (leftHand->Type() == NodeType::BINARY_OP)
		leftHand = _ExecuteBinaryOperation(static_cast<BinaryOperation*>(leftHand), scope);
	else if (leftHand->Type() == NodeType::ARRAY_ELEMENT_IDENTIFIER)
		leftHand = _GetArrayElement(static_cast<ArrayElementIdentifierNode*>(leftHand), scope);
	// For now, check if the left hand type is not a
	// operatable type, such as a string or an
	// integer, and if not, throw an error
	else if (
		leftHand->Type() != NodeType::INTEGER &&
		leftHand->Type() != NodeType::DECIMAL &&
		leftHand->Type() != NodeType::STRING  &&
		leftHand->Type() != NodeType::BOOLEAN
	)
	{
		std::cerr << "Interpreter Error! Unrecognized type in left hand of operation!" << std::endl;

		if (m_errorHandle)
			m_errorHandle();

		return nullptr;
	}

	if (rightHand->Type() == NodeType::IDENTIFIER)
		rightHand = scope.Search(static_cast<IdentifierNode*>(rightHand)->Value());
	// Nested binary operations should be handled recursively
	else if (rightHand->Type() == NodeType::BINARY_OP)
		rightHand = _ExecuteBinaryOperation(static_cast<BinaryOperation*>(rightHand), scope);
	else if (rightHand->Type() == NodeType::CALL)
		rightHand = _ExecuteFunction(static_cast<FunctionCall*>(rightHand), scope);
	else if (rightHand->Type() == NodeType::ARRAY_ELEMENT_IDENTIFIER)
		rightHand = _GetArrayElement(static_cast<ArrayElementIdentifierNode*>(rightHand), scope);
	// Do the same as with the left hand, check if it is not
	// an operatable type, and if so throw an error
	else if (
		rightHand->Type() != NodeType::INTEGER &&
		rightHand->Type() != NodeType::DECIMAL &&
		rightHand->Type() != NodeType::STRING  &&
		rightHand->Type() != NodeType::BOOLEAN
	)
	{
		std::cerr << "Interpreter Error! Unrecognized type in right hand of operation!" << std::endl;

		if (m_errorHandle)
			m_errorHandle();

		return nullptr;
	}

	bool isComparison = IsLogicalOperation(operation->Operation());

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
		 rightHand->Type() != NodeType::STRING) &&
		!isComparison
	)
	{
		result = new DecimalNode();
		result->Type(NodeType::DECIMAL);
	}
	else if (
		leftHand->Type() == NodeType::INTEGER &&
		rightHand->Type() == NodeType::INTEGER &&
		!isComparison
	)
	{
		result = new IntegerNode();
		result->Type(NodeType::INTEGER);
	}
	else if (
		leftHand->Type() == NodeType::STRING  &&
		rightHand->Type() == NodeType::STRING &&
		!isComparison
	)
	{
		result = new StringNode();
		result->Type(NodeType::STRING);
	}
	else if (
		isComparison &&
		leftHand->Type() == rightHand->Type()
	)
	{
		result = new BooleanNode();
		result->Type(NodeType::BOOLEAN);
	}
	else
	{
		std::cerr << "Interpreter Error! Mismatched type for operation." << std::endl;
		std::cerr << "Left-hand type: " << GetNodeTypeName(leftHand->Type()) << ";";
		std::cerr << " Right-hand type: " << GetNodeTypeName(rightHand->Type()) << std::endl;

		delete leftHand;
		delete rightHand;

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

			delete leftHand;
			delete rightHand;

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

			delete leftHand;
			delete rightHand;

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
	case NodeType::MODULUS:
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

			static_cast<DecimalNode*>(result)->Value(false, fmod(leftVal, rightVal));

			return result;
		}
		else if (result->Type() == NodeType::INTEGER)
		{
			static_cast<IntegerNode*>(result)->Value(
				false,
				static_cast<IntegerNode*>(leftHand)->Value() % static_cast<IntegerNode*>(rightHand)->Value()
			);

			return result;
		}
		else if (result->Type() == NodeType::STRING)
		{
			std::cerr << "Interpreter Error! Cannot modulus a string!" << std::endl;

			if (m_errorHandle)
				m_errorHandle();

			return nullptr;
		}

		break;
	}
	case NodeType::EQUAL_COMPARISON:
	{
		// Since we already checked if the types
		// are the same between the two sides
		// we can switch on the left hand type
		// and cast both
		switch (leftHand->Type())
		{
		case NodeType::INTEGER:
		{
			IntegerNode* leftInt = static_cast<IntegerNode*>(leftHand);
			IntegerNode* rightInt = static_cast<IntegerNode*>(rightHand);

			static_cast<BooleanNode*>(result)->Value(leftInt->Value() == rightInt->Value());

			break;
		}
		case NodeType::DECIMAL:
		{
			DecimalNode* leftDec = static_cast<DecimalNode*>(leftHand);
			DecimalNode* rightDec = static_cast<DecimalNode*>(rightHand);

			static_cast<BooleanNode*>(result)->Value(leftDec->Value() == rightDec->Value());

			break;
		}
		case NodeType::STRING:
		{
			StringNode* leftStr = static_cast<StringNode*>(leftHand);
			StringNode* rightStr = static_cast<StringNode*>(rightHand);

			static_cast<BooleanNode*>(result)->Value(leftStr->Value() == rightStr->Value());

			break;
		}
		case NodeType::BOOLEAN:
		{
			BooleanNode* leftBool = static_cast<BooleanNode*>(leftHand);
			BooleanNode* rightBool = static_cast<BooleanNode*>(rightHand);

			static_cast<BooleanNode*>(result)->Value(leftBool->Value() == rightBool->Value());

			break;
		}
		}

		return result;
	}
	case NodeType::NOTEQUAL_COMPARISON:
	{
		// Since we already checked if the types
		// are the same between the two sides
		// we can switch on the left hand type
		// and cast both
		switch (leftHand->Type())
		{
		case NodeType::INTEGER:
		{
			IntegerNode* leftInt = static_cast<IntegerNode*>(leftHand);
			IntegerNode* rightInt = static_cast<IntegerNode*>(rightHand);

			static_cast<BooleanNode*>(result)->Value(leftInt->Value() != rightInt->Value());

			break;
		}
		case NodeType::DECIMAL:
		{
			DecimalNode* leftDec = static_cast<DecimalNode*>(leftHand);
			DecimalNode* rightDec = static_cast<DecimalNode*>(rightHand);

			static_cast<BooleanNode*>(result)->Value(leftDec->Value() != rightDec->Value());

			break;
		}
		case NodeType::STRING:
		{
			StringNode* leftStr = static_cast<StringNode*>(leftHand);
			StringNode* rightStr = static_cast<StringNode*>(rightHand);

			static_cast<BooleanNode*>(result)->Value(leftStr->Value() != rightStr->Value());

			break;
		}
		case NodeType::BOOLEAN:
		{
			BooleanNode* leftBool = static_cast<BooleanNode*>(leftHand);
			BooleanNode* rightBool = static_cast<BooleanNode*>(rightHand);

			static_cast<BooleanNode*>(result)->Value(leftBool->Value() != rightBool->Value());

			break;
		}
		}

		return result;
	}
	case NodeType::LESS_COMPARISON:
	{
		// Since we already checked if the types
		// are the same between the two sides
		// we can switch on the left hand type
		// and cast both
		switch (leftHand->Type())
		{
		case NodeType::INTEGER:
		{
			IntegerNode* leftInt  = static_cast<IntegerNode*>(leftHand);
			IntegerNode* rightInt = static_cast<IntegerNode*>(rightHand);

			static_cast<BooleanNode*>(result)->Value(leftInt->Value() < rightInt->Value());

			break;
		}
		case NodeType::DECIMAL:
		{
			DecimalNode* leftDec  = static_cast<DecimalNode*>(leftHand);
			DecimalNode* rightDec = static_cast<DecimalNode*>(rightHand);

			static_cast<BooleanNode*>(result)->Value(leftDec->Value() < rightDec->Value());

			break;
		}
		default:
		{
			std::cerr << "Interpreter Error! Less operator not supported for type used." << std::endl;
			std::cerr << "Type: " << GetNodeTypeName(leftHand->Type()) << std::endl;

			if (m_errorHandle)
				m_errorHandle();

			return nullptr;
		}
		}

		return result;
	}
	case NodeType::GREATER_COMPARISON:
	{
		// Since we already checked if the types
		// are the same between the two sides
		// we can switch on the left hand type
		// and cast both
		switch (leftHand->Type())
		{
		case NodeType::INTEGER:
		{
			IntegerNode* leftInt  = static_cast<IntegerNode*>(leftHand);
			IntegerNode* rightInt = static_cast<IntegerNode*>(rightHand);

			static_cast<BooleanNode*>(result)->Value(leftInt->Value() > rightInt->Value());

			break;
		}
		case NodeType::DECIMAL:
		{
			DecimalNode* leftDec  = static_cast<DecimalNode*>(leftHand);
			DecimalNode* rightDec = static_cast<DecimalNode*>(rightHand);

			static_cast<BooleanNode*>(result)->Value(leftDec->Value() > rightDec->Value());

			break;
		}
		default:
		{
			std::cerr << "Interpreter Error! Greater operator not supported for type used." << std::endl;
			std::cerr << "Type: " << GetNodeTypeName(leftHand->Type()) << std::endl;

			if (m_errorHandle)
				m_errorHandle();

			return nullptr;
		}
		}

		return result;
	}
	case NodeType::LESSEQUAL_COMPARISON:
	{
		// Since we already checked if the types
		// are the same between the two sides
		// we can switch on the left hand type
		// and cast both
		switch (leftHand->Type())
		{
		case NodeType::INTEGER:
		{
			IntegerNode* leftInt  = static_cast<IntegerNode*>(leftHand);
			IntegerNode* rightInt = static_cast<IntegerNode*>(rightHand);

			static_cast<BooleanNode*>(result)->Value(leftInt->Value() <= rightInt->Value());

			break;
		}
		case NodeType::DECIMAL:
		{
			DecimalNode* leftDec  = static_cast<DecimalNode*>(leftHand);
			DecimalNode* rightDec = static_cast<DecimalNode*>(rightHand);

			static_cast<BooleanNode*>(result)->Value(leftDec->Value() <= rightDec->Value());

			break;
		}
		default:
		{
			std::cerr << "Interpreter Error! Less equal operator not supported for type used." << std::endl;
			std::cerr << "Type: " << GetNodeTypeName(leftHand->Type()) << std::endl;

			if (m_errorHandle)
				m_errorHandle();

			return nullptr;
		}
		}

		return result;
	}
	case NodeType::GREATEREQUAL_COMPARISON:
	{
		// Since we already checked if the types
		// are the same between the two sides
		// we can switch on the left hand type
		// and cast both
		switch (leftHand->Type())
		{
		case NodeType::INTEGER:
		{
			IntegerNode* leftInt = static_cast<IntegerNode*>(leftHand);
			IntegerNode* rightInt = static_cast<IntegerNode*>(rightHand);

			static_cast<BooleanNode*>(result)->Value(leftInt->Value() >= rightInt->Value());

			break;
		}
		case NodeType::DECIMAL:
		{
			DecimalNode* leftDec = static_cast<DecimalNode*>(leftHand);
			DecimalNode* rightDec = static_cast<DecimalNode*>(rightHand);

			static_cast<BooleanNode*>(result)->Value(leftDec->Value() >= rightDec->Value());

			break;
		}
		default:
		{
			std::cerr << "Interpreter Error! Greater equal operator not supported for type used." << std::endl;
			std::cerr << "Type: " << GetNodeTypeName(leftHand->Type()) << std::endl;

			if (m_errorHandle)
				m_errorHandle();

			return nullptr;
		}
		}

		return result;
	}
	default:
	{
		std::cerr << "Interpreter Error! Unrecognized operation type!" << std::endl;
		std::cerr << "Operation type: " << GetNodeTypeName(operation->Operation()) << std::endl;

		if (m_errorHandle)
			m_errorHandle();

		return nullptr;
	}
	}

	return nullptr;
}

BooleanNode* Interpreter::_ExecuteLogicalOperation(
	BinaryOperation* operation,
	Scope&                scope
)
{
	// Set up the two sides of the logical operation
	// as well as the resulting boolean
	Node* leftHand      = operation->GetLeftHand();
	Node* rightHand     = operation->GetRightHand();
	BooleanNode* result = nullptr;

	// Grab the left hand side from the current scope
	// checking if this is actually a boolean type will
	// be later
	if (leftHand->Type() == NodeType::IDENTIFIER)
		leftHand = scope.Search(static_cast<IdentifierNode*>(leftHand)->Value());
	else if (leftHand->Type() == NodeType::CALL)
		leftHand = _ExecuteFunction(static_cast<FunctionCall*>(leftHand), scope);
	else if (leftHand->Type() == NodeType::BINARY_OP)
		leftHand = _ExecuteBinaryOperation(static_cast<BinaryOperation*>(leftHand), scope);
	else if (leftHand->Type() == NodeType::ARRAY_ELEMENT_IDENTIFIER)
		leftHand = _GetArrayElement(static_cast<ArrayElementIdentifierNode*>(leftHand), scope);
	// For now, make sure that the left hand side is a
	// boolean to check with
	else if (leftHand->Type() != NodeType::BOOLEAN)
	{
		std::cerr << "Interpreter Error! Unsupported or recognized type in left hand of logical operation!" << std::endl;
		std::cerr << "Actual type: " << GetNodeTypeName(leftHand->Type()) << std::endl;

		if (m_errorHandle)
			m_errorHandle();

		return nullptr;
	}

	// Make sure that no non-boolean slipped past us
	if (leftHand->Type() != NodeType::BOOLEAN)
	{
		std::cerr << "Interpreter Error! Non-boolean type in left hand of logical operation!" << std::endl;
		std::cerr << "Actual type: " << GetNodeTypeName(leftHand->Type()) << std::endl;

		if (m_errorHandle)
			m_errorHandle();

		return nullptr;
	}

	// If the left hand side is false and we have an AND logic
	// do not continue, and return false
	if (operation->Operation() == NodeType::AND && !(static_cast<BooleanNode*>(leftHand)->Value()))
	{
		result = new BooleanNode();
		result->Type(NodeType::BOOLEAN);

		result->Value(false);

		return result;
	}

	// Now we grab the right hand side
	if (rightHand->Type() == NodeType::IDENTIFIER)
		rightHand = scope.Search(static_cast<IdentifierNode*>(rightHand)->Value());
	else if (rightHand->Type() == NodeType::CALL)
		rightHand = _ExecuteFunction(static_cast<FunctionCall*>(rightHand), scope);
	else if (rightHand->Type() == NodeType::BINARY_OP)
		rightHand = _ExecuteBinaryOperation(static_cast<BinaryOperation*>(rightHand), scope);
	else if (rightHand->Type() == NodeType::LOGICAL_OP)
		rightHand = _ExecuteLogicalOperation(static_cast<BinaryOperation*>(rightHand), scope);
	else if (rightHand->Type() == NodeType::ARRAY_ELEMENT_IDENTIFIER)
		rightHand = _GetArrayElement(static_cast<ArrayElementIdentifierNode*>(rightHand), scope);
	// For now, check if the right hand type is at least
	// a boolean for checking
	else if (rightHand->Type() != NodeType::BOOLEAN)
	{
		std::cerr << "Interpreter Error! Unsupported or recognized type in right hand of logical operation!" << std::endl;
		std::cerr << "Actual type: " << GetNodeTypeName(rightHand->Type()) << std::endl;

		if (m_errorHandle)
			m_errorHandle();

		return nullptr;
	}

	// Make sure that no non-boolean slipped past us
	if (rightHand->Type() != NodeType::BOOLEAN)
	{
		std::cerr << "Interpreter Error! Non-boolean type in right hand of logical operation!" << std::endl;
		std::cerr << "Actual type: " << GetNodeTypeName(rightHand->Type()) << std::endl;

		if (m_errorHandle)
			m_errorHandle();

		return nullptr;
	}

	// If this is an AND logical operation, and the right
	// hand is true (the left hand is already also known
	// to be true, return a true boolean value
	if (operation->Operation() == NodeType::AND && static_cast<BooleanNode*>(rightHand)->Value())
	{
		result = new BooleanNode();
		result->Type(NodeType::BOOLEAN);

		result->Value(true);

		return result;
	}
	// Otherwise, if it is false and the operation is an AND, return false
	else if (operation->Operation() == NodeType::AND && !(static_cast<BooleanNode*>(rightHand)->Value()))
	{
		result = new BooleanNode();
		result->Type(NodeType::BOOLEAN);

		result->Value(false);

		return result;
	}
	// Otherwise, this can be an OR logical operation, and if either of the
	// values are true, return true
	else if (operation->Operation() == NodeType::OR && (static_cast<BooleanNode*>(leftHand)->Value() || static_cast<BooleanNode*>(rightHand)->Value()))
	{
		result = new BooleanNode();
		result->Type(NodeType::BOOLEAN);

		result->Value(true);

		return result;
	}
	// Otherwise, the OR has failed, and we can return false
	else if (operation->Operation() == NodeType::OR)
	{
		result = new BooleanNode();
		result->Type(NodeType::BOOLEAN);

		result->Value(false);

		return result;
	}

	return nullptr;
}

AST::Node* Interpreter::_GetArrayElement(
	AST::ArrayElementIdentifierNode* identifier,
	Scope&                           scope
)
{
	// First, try to grab the array
	// from the string identifier
	const std::string arrayIdent = identifier->ArrayIdentifier();

	// Try and find this array
	Node* arrayNode = scope.Search(arrayIdent);

	if (!arrayNode)
	{
		std::cerr << "Interpreter Error! Undeclared identifier \"" << arrayIdent << "\"!";

		if (m_errorHandle)
			m_errorHandle();
	
		return nullptr;
	}

	if (arrayNode->Type() != NodeType::ARRAY)
	{
		std::cerr << "Interpreter Error! Cannot use array access syntax on a non array!" << std::endl;
		std::cerr << "Actual type: " << GetNodeTypeName(arrayNode->Type()) << std::endl;

		if (m_errorHandle)
			m_errorHandle();
	}

	// Now, evaluate the index used
	// for the identifier
	Node* index = identifier->Index()->Clone();

	if (index->Type() == NodeType::IDENTIFIER)
		index = scope.Search(static_cast<IdentifierNode*>(index)->Value())->Clone();
	else if (index->Type() == NodeType::BINARY_OP)
		index = _ExecuteBinaryOperation(static_cast<BinaryOperation*>(index), scope);
	else if (index->Type() == NodeType::LOGICAL_OP)
		index = _ExecuteLogicalOperation(static_cast<BinaryOperation*>(index), scope);
	else if (index->Type() == NodeType::CALL)
		index = _ExecuteFunction(static_cast<FunctionCall*>(index), scope);
	else if (index->Type() == NodeType::ARRAY_ELEMENT_IDENTIFIER)
		index = _GetArrayElement(static_cast<ArrayElementIdentifierNode*>(index), scope)->Clone();

	if (!index)
	{
		std::cerr << "Interpreter Error! Undefined index for array access!" << std::endl;

		if (m_errorHandle)
			m_errorHandle();
	}

	if (index->Type() != NodeType::INTEGER)
	{
		std::cerr << "Interpreter Error! Index for array access must be an integer!" << std::endl;
		std::cerr << "Actual Type: " << GetNodeTypeName(index->Type());

		delete index;

		if (m_errorHandle)
			m_errorHandle();
	}

	// Now, grab the index as a size_t
	int32_t arrIndex = static_cast<IntegerNode*>(index)->Value();

	// The index node is no longer needed, delete
	delete index;

	// Check if the size is within bounds
	if (arrIndex < 0 || static_cast<size_t>(arrIndex) >= static_cast<ArrayNode*>(arrayNode)->Size())
	{
		std::cerr << "Interpreter Error! Array index out of bounds!" << std::endl;
		std::cerr << "Index: " << arrIndex << "; Size: " << static_cast<ArrayNode*>(arrayNode)->Size() << std::endl;
	
		if (m_errorHandle)
			m_errorHandle();
	}

	// Finally, return the actual node
	return (*static_cast<ArrayNode*>(arrayNode))[arrIndex];
}

void Interpreter::_CreateCommandArgumentsArray()
{
	// Shrink the arguments vector before
	// anything else
	m_arguments.shrink_to_fit();

	// Create the array node
	ArrayNode* cmdArgsArray = new ArrayNode();
	cmdArgsArray->Type(NodeType::ARRAY);

	// Create the capacity for the array
	IntegerNode* capacity = new IntegerNode();
	capacity->Type(NodeType::INTEGER);

	capacity->Value(false, m_arguments.size());

	capacity->Parent(cmdArgsArray);

	cmdArgsArray->Fixed(true);
	cmdArgsArray->Capacity(capacity);

	// Now, fill the array with the
	// string nodes
	for (size_t index = 0; index < m_arguments.size(); index++)
	{
		StringNode* argument = new StringNode();
		argument->Type(NodeType::STRING);

		argument->Value(m_arguments[index]);

		cmdArgsArray->Insert(argument);
	}

	// Now, insert it into global scope with
	// the reserved array name defined in
	// the precompiled header
	m_globalScope.Insert(SHAKARA_CMD_ARGS_NAME, cmdArgsArray);
}