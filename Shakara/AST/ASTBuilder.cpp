#include "../stdafx.hpp"
#include "ASTBuilder.hpp"

#include "ASTTypes.hpp"
#include "ASTNode.hpp"
#include "Nodes/ASTAssignmentNode.hpp"
#include "Nodes/ASTIntegerNode.hpp"
#include "Nodes/ASTBinaryOperation.hpp"
#include "Nodes/ASTIdentifierNode.hpp"
#include "Nodes/ASTRootNode.hpp"
#include "Nodes/ASTFunctionDeclarationNode.hpp"
#include "Nodes/ASTFunctionCallNode.hpp"
#include "Nodes/ASTDecimalNode.hpp"
#include "Nodes/ASTStringNode.hpp"

#include "../Tokenizer/TokenizerTypes.hpp"

using namespace Shakara;
using namespace Shakara::AST;

void ASTBuilder::Build(
	RootNode*           root,
	std::vector<Token>& tokens,
	size_t              index
)
{
	// Build a new node from the tokens at
	// the current index
	ptrdiff_t next = index + 1;

	_BuildIndividualNode(
		root,
		tokens,
		index,
		&next
	);

	// We still have more portions of the AST to build
	// thus, continue building
	//
	// Otherwise, do some memory saving by resizing
	// the RootNode vector to fit each child without
	// any extra space
	//
	// Though, stop recursively building once we hit
	// a end block token if the flag is set
	if ((next > 0 && static_cast<size_t>(next) < tokens.size()))
		Build(
			root,
			tokens,
			next
		);
	else
		root->ShrinkToFit();
}

bool ASTBuilder::_BuildIndividualNode(
	RootNode*           root,
	std::vector<Token>& tokens,
	size_t              index,
	ptrdiff_t*          next
)
{
	const Token& token = tokens[index];

	// Check if there is anything to do with
	// an identifier first, as most portions
	// of the language deal with identifiers
	if (token.type == TokenType::IDENTIFIER)
	{
		// Check if there's an equal sign
		// after this identifier, and if
		// so, it's an assignment
		if (tokens[index + 1].type == TokenType::EQUAL)
		{
			// Attempt to parse either a function definition
			// or a variable assignment
			//
			// Differentiated by whether or not BEGIN_ARGS is
			// after the equal sign or not
			if (tokens[index + 2].type == TokenType::BEGIN_ARGS)
			{
				_ParseFunctionDefinition(
					root,
					tokens,
					index,
					next
				);
			
				return false;
			}
			else
			{
				_ParseVariableAssignment(
					root,
					tokens,
					index,
					next
				);
			
				return true;
			}
		}
		// For either increment or decrement variables
		else if (
			tokens[index + 1].type == TokenType::INCREMENT ||
			tokens[index + 1].type == TokenType::DECREMENT
		)
		{
			_ParseVariableIncrementDecrement(
				root,
				tokens,
				index,
				next
			);

			return true;
		}
		// For any assignment arithmetic operators
		// such as += or -=
		else if (
			tokens[index + 1].type == TokenType::PLUS_EQUAL     ||
			tokens[index + 1].type == TokenType::MINUS_EQUAL    ||
			tokens[index + 1].type == TokenType::MULTIPLY_EQUAL ||
			tokens[index + 1].type == TokenType::DIVIDE_EQUAL
		)
		{
			_ParseVariableArithmeticAssignment(
				root,
				tokens,
				index,
				next
			);

			return true;
		}
		// Check if this could be a function call
		else if (tokens[index + 1].type == TokenType::BEGIN_ARGS)
		{
			_ParseFunctionCall(
				root,
				tokens,
				index,
				next
			);

			return true;
		}
	}

	return false;
}

void ASTBuilder::_ParseVariableAssignment(
	RootNode*           root,
	std::vector<Token>& tokens,
	size_t              index,
	ptrdiff_t*			next
)
{
	// Start by setting the next token
	// as the index passed in
	*next = index;

	// Create the assignment node to be
	// added to the AST
	AssignmentNode* assignment = new AssignmentNode();
	assignment->Type(NodeType::ASSIGN);

	// Set the identifier using the
	// index as the first node
	IdentifierNode* identifier = new IdentifierNode();
	identifier->Type(NodeType::IDENTIFIER);

	identifier->Value(tokens[(*next)].value);

	identifier->Parent(assignment);

	assignment->Identifier(identifier);

	(*next)++;

	// Move on to the equal sign, which
	// should just be skipped, as we already
	// know that this is an assignment
	(*next)++;

	// Now for the harder part, assignment
	// can contain any number of operations
	// or a lack of an operation, we must
	// lookahead to find if this is a simple
	// one value assignment or a operation
	if (static_cast<size_t>((*next) + 1) < tokens.size() && IsArithmeticType(tokens[(*next) + 1].type))
	{
		BinaryOperation* operation = new BinaryOperation();
		operation->Type(NodeType::BINARY_OP);

		_ParseBinaryOperation(
			operation,
			tokens,
			*next,
			next
		);

		operation->Parent(assignment);

		assignment->Assignment(operation);
	}
	// There is no other nested operation to do,
	// now check if this type is a identifier or
	// a number of sorts
	else if (
		tokens[*next].type == TokenType::IDENTIFIER ||
		tokens[*next].type == TokenType::INTEGER    ||
		tokens[*next].type == TokenType::DECIMAL    ||
		tokens[*next].type == TokenType::STRING
	)
	{
		Node* value = nullptr;
		_CreateSingleNodeFromToken(
			&value,
			tokens[(*next)]
		);

		value->Parent(assignment);

		assignment->Assignment(value);

		(*next)++;
	}

	root->Insert(assignment);
}

void ASTBuilder::_ParseFunctionCall(
	RootNode*           root,
	std::vector<Token>& tokens,
	size_t              index,
	ptrdiff_t*			next
)
{
	// Start by setting the next token
	// as the index passed in
	*next = index;

	// Create the function call node for
	// addition to the AST
	FunctionCall* call = new FunctionCall();
	call->Type(NodeType::CALL);

	// Set the identifier as the first token
	// at the passed in index
	IdentifierNode* identifier = new IdentifierNode();
	identifier->Type(NodeType::IDENTIFIER);

	identifier->Value(tokens[(*next)].value);

	identifier->Parent(call);

	call->Identifier(identifier);

	// We know that there's a begin args
	// after this identifier, so just
	// run next twice to skip it
	(*next)++;
	(*next)++;

	// Now, we have a while loop to try
	// and grab each argument inside of
	// the call
	while (static_cast<size_t>((*next)) < tokens.size())
	{
		if (tokens[*next].type == TokenType::END_ARGS)
		{
			(*next)++;

			break;
		}

		// Do roughly the same thing we've been doing
		// for anything that could have a binary operation
		// or just a regular identifier/number
		// Now for the harder part, assignment
		// can contain any number of operations
		// or a lack of an operation, we must
		// lookahead to find if this is a simple
		// one value assignment or a operation
		if (
			static_cast<size_t>((*next) + 1) < tokens.size() &&
			IsArithmeticType(tokens[(*next) + 1].type)
		)
		{
			BinaryOperation* operation = new BinaryOperation();
			operation->Type(NodeType::BINARY_OP);

			_ParseBinaryOperation(
				operation,
				tokens,
				*next,
				next
			);

			call->InsertArgument(operation);

			continue;
		}
		// There is no other nested operation to do,
		// now check if this type is a identifier or
		// a number of sorts
		else if (
			tokens[*next].type == TokenType::IDENTIFIER ||
			tokens[*next].type == TokenType::INTEGER    ||
			tokens[*next].type == TokenType::DECIMAL    ||
			tokens[*next].type == TokenType::STRING
		)
		{
			Node* value = nullptr;
			_CreateSingleNodeFromToken(
				&value,
				tokens[(*next)]
			);

			call->InsertArgument(value);
		}

		(*next)++;
	}

	root->Insert(call);
}

void ASTBuilder::_ParseVariableIncrementDecrement(
	RootNode*           root,
	std::vector<Token>& tokens,
	size_t              index,
	ptrdiff_t*          next
)
{
	// Start by setting the next token
	// as the index passed in
	*next = index;

	// Create an assignment node to increment
	// the variable
	//
	// Technically I could create an increment
	// or deincrement node, but it seems nicer
	// to me to just implement it as an assignment
	// to a binary operation
	AssignmentNode* assignment = new AssignmentNode();
	assignment->Type(NodeType::ASSIGN);

	// Create the IdentifierNode to assign the
	// increment or deincrement to
	IdentifierNode* identifier = new IdentifierNode();
	identifier->Type(NodeType::IDENTIFIER);

	identifier->Value(tokens[(*next)].value);

	identifier->Parent(assignment);

	assignment->Identifier(identifier);

	(*next)++;

	// Create the binary operation for this
	// operation
	BinaryOperation* operation = new BinaryOperation();
	operation->Type(NodeType::BINARY_OP);

	// Create the left hand side of the operation
	// with a copy of the current identifier
	//
	// This is kind of a hacky workaround, as each
	// node recursively deletes, and will try to
	// double delete if I use the same identifier
	// instance
	operation->LeftHand(new IdentifierNode(*identifier));

	// Figure out if this is a increment or
	// a decrement so that I can assign a
	// correct binary operation
	if (tokens[*next].type == TokenType::INCREMENT)
		operation->Operation(NodeType::ADD);
	else if(tokens[*next].type == TokenType::DECREMENT)
		operation->Operation(NodeType::SUBTRACT);

	// Move on for the next call
	(*next)++;

	// TODO: Throw an error if the type is incorrect

	// Create the right hand of the binary operation
	//
	// Initialize this to one as we are only incrementing
	// or decrementing by one
	IntegerNode* one = new IntegerNode();
	one->Value(false, 1);

	operation->RightHand(one);

	operation->Parent(assignment);

	assignment->Assignment(operation);

	root->Insert(assignment);
}

void ASTBuilder::_ParseVariableArithmeticAssignment(
	RootNode*           root,
	std::vector<Token>& tokens,
	size_t              index,
	ptrdiff_t*          next
)
{
	// Start by setting the next token
	// as the index passed in
	*next = index;

	// Create an assignment node to increment
	// the variable
	//
	// The reason for using an AssignmentNode
	// as opposed to a new node type for these
	// operators is for code and project simplicity
	AssignmentNode* assignment = new AssignmentNode();
	assignment->Type(NodeType::ASSIGN);

	// Create the IdentifierNode to assign the
	// increment or deincrement to
	IdentifierNode* identifier = new IdentifierNode();
	identifier->Type(NodeType::IDENTIFIER);

	identifier->Value(tokens[(*next)].value);

	identifier->Parent(assignment);

	assignment->Identifier(identifier);

	(*next)++;

	// Create the binary operation for this
	// operation
	BinaryOperation* operation = new BinaryOperation();
	operation->Type(NodeType::BINARY_OP);

	// Create the left hand side of the operation
	// with a copy of the current identifier
	//
	// This is kind of a hacky workaround, as each
	// node recursively deletes, and will try to
	// double delete if I use the same identifier
	// instance
	operation->LeftHand(new IdentifierNode(*identifier));

	// Figure out if this is a increment or
	// a decrement so that I can assign a
	// correct binary operation
	if (tokens[*next].type == TokenType::PLUS_EQUAL)
		operation->Operation(NodeType::ADD);
	else if (tokens[*next].type == TokenType::MINUS_EQUAL)
		operation->Operation(NodeType::SUBTRACT);
	else if (tokens[*next].type == TokenType::MULTIPLY_EQUAL)
		operation->Operation(NodeType::MULTIPLY);
	else if (tokens[*next].type == TokenType::DIVIDE_EQUAL)
		operation->Operation(NodeType::DIVIDE);

	// Move on for the next call
	(*next)++;

	// TODO: Throw an error if the type is incorrect

	// Now to parse what to insert into the right hand
	// side of the binary operation.
	//
	// This could be another binary operation or just
	// a number of some type.
	//
	// This code is almost identical to the code used for
	// parsing a regular variable assignment.
	if (static_cast<size_t>((*next) + 1) < tokens.size() && IsArithmeticType(tokens[(*next) + 1].type))
	{
		BinaryOperation* nestedOp = new BinaryOperation();
		nestedOp->Type(NodeType::BINARY_OP);

		_ParseBinaryOperation(
			nestedOp,
			tokens,
			*next,
			next
		);

		nestedOp->Parent(operation);
	
		operation->RightHand(nestedOp);
	}
	// There is no other nested operation to do,
	// now check if this type is a identifier or
	// a number of sorts
	else if (
		tokens[*next].type == TokenType::IDENTIFIER ||
		tokens[*next].type == TokenType::INTEGER    ||
		tokens[*next].type == TokenType::DECIMAL    ||
		tokens[*next].type == TokenType::STRING
		)
	{
		Node* value = nullptr;
		_CreateSingleNodeFromToken(
			&value,
			tokens[(*next)]
		);

		value->Parent(operation);

		operation->RightHand(value);

		(*next)++;
	}

	assignment->Assignment(operation);

	root->Insert(assignment);
}

void ASTBuilder::_ParseFunctionDefinition(
	RootNode*           root,
	std::vector<Token>& tokens,
	size_t              index,
	ptrdiff_t*          next
)
{
	// Start by setting the next token
	// as the index passed in
	*next = index;

	// Create the function declaration node
	// to be added to the root
	FunctionDeclaration* declaration = new FunctionDeclaration();
	declaration->Type(NodeType::FUNCTION);

	// Set the identifier using the
	// index as the first node
	IdentifierNode* identifier = new IdentifierNode();
	identifier->Type(NodeType::IDENTIFIER);

	identifier->Value(tokens[*next].value);

	identifier->Parent(declaration);

	declaration->Identifier(identifier);

	// Move on to trying to parse arguments
	(*next)++;

	// Move on if this token is an equal sign
	if (tokens[*next].type == TokenType::EQUAL)
		(*next)++;

	// Move on if this token is the beginning
	// of the args
	if (tokens[*next].type == TokenType::BEGIN_ARGS)
		(*next)++;

	// Start parsing arguments by using a
	// while loop until an END_ARGS is found
	// or until the end of the tokens
	while (static_cast<size_t>((*next)) < tokens.size())
	{
		if (tokens[*next].type == TokenType::END_ARGS)
		{
			(*next)++;

			break;
		}

		if (tokens[*next].type != TokenType::IDENTIFIER)
		{
			// TODO: Throw an error here!
			
			(*next)++;

			continue;
		}

		IdentifierNode* argument = new IdentifierNode();
		argument->Type(NodeType::IDENTIFIER);

		argument->Value(tokens[*next].value);

		argument->Parent(declaration);

		declaration->InsertArgument(argument);

		(*next)++;
	}

	// Once we are done with the arguments, check if there's a
	// BEGIN_BLOCK token at the current location
	if (tokens[*next].type == TokenType::BEGIN_BLOCK)
		(*next)++;

	// Now, we can do the same kind of while loop for the
	// arguments, but for statements within the body
	RootNode* body = new RootNode();
	
	while (static_cast<size_t>((*next)) < tokens.size())
	{
		if (tokens[*next].type == TokenType::END_BLOCK)
			break;

		// Attempt to build a new node for the
		// function, if one is not able to be
		// made, continue to the next token
		if (!_BuildIndividualNode(
			body,
			tokens,
			*next,
			next
		))
			(*next)++;
	}

	// Now add the body statements to the declaration
	// and add the declaration to the root
	declaration->Body(body);

	root->Insert(declaration);
}

void ASTBuilder::_ParseBinaryOperation(
	BinaryOperation*    operation,
	std::vector<Token>& tokens,
	size_t              index,
	ptrdiff_t*          next
)
{
	// Start by setting the next token
	// as the index passed in
	*next = index;

	// Determine the type of node
	// to create for the left hand
	// side of the operation
	Node* leftHand = nullptr;
	_CreateSingleNodeFromToken(
		&leftHand,
		tokens[index]
	);

	operation->LeftHand(leftHand);

	// Now get the operation type
	(*next)++;
	const TokenType& type = tokens[*next].type;
	NodeType         op = NodeType::ROOT;

	switch (type)
	{
	case TokenType::PLUS:
		op = NodeType::ADD;
		break;
	case TokenType::MINUS:
		op = NodeType::SUBTRACT;
		break;
	case TokenType::MULTIPLY:
		op = NodeType::MULTIPLY;
		break;
	case TokenType::DIVIDE:
		op = NodeType::DIVIDE;
		break;
	default:
		// TODO: Throw an error
		break;
	}

	operation->Operation(op);

	(*next)++;

	// This is more tricky, since there can be
	// more operations after this one, we have
	// to perform a lookahead to find if there's
	// an extra operation, if not, just insert
	// another single node
	//
	// Otherwise, recursively run this function
	if (static_cast<size_t>((*next) + 1) < tokens.size() && IsArithmeticType(tokens[(*next) + 1].type))
	{
		BinaryOperation* nestedOp = new BinaryOperation();
		nestedOp->Type(NodeType::BINARY_OP);

		_ParseBinaryOperation(
			nestedOp,
			tokens,
			*next,
			next
		);

		nestedOp->Parent(operation);

		operation->RightHand(nestedOp);
	}
	// Parse another single node
	else
	{
		Node* rightHand = nullptr;
		_CreateSingleNodeFromToken(
			&rightHand,
			tokens[*next]
		);

		rightHand->Parent(operation);

		operation->RightHand(rightHand);

		(*next)++;
	}
}

inline void ASTBuilder::_CreateSingleNodeFromToken(
	Node**       node,
	const Token& token
)
{
	switch (token.type)
	{
	case TokenType::IDENTIFIER:
	{
		*node = new IdentifierNode();
		static_cast<IdentifierNode*>(*node)->Value(token.value);

		(*node)->Type(NodeType::IDENTIFIER);

		break;
	}
	case TokenType::INTEGER:
	{
		*node = new IntegerNode();
		static_cast<IntegerNode*>(*node)->Value(false, static_cast<int32_t>(std::stoi(token.value)));

		(*node)->Type(NodeType::INTEGER);

		break;
	}
	case TokenType::DECIMAL:
	{
		*node = new DecimalNode();
		static_cast<DecimalNode*>(*node)->Value(false, static_cast<float>(std::stof(token.value)));

		(*node)->Type(NodeType::DECIMAL);

		break;
	}
	case TokenType::STRING:
	{
		*node = new StringNode();
		static_cast<StringNode*>(*node)->Value(token.value);

		(*node)->Type(NodeType::STRING);

		break;
	}
	default:
		break;
	}
}