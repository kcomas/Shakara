#include "../stdafx.hpp"
#include "ASTBuilder.hpp"

#include "ASTTypes.hpp"
#include "ASTNode.hpp"
#include "Nodes/ASTAssignmentNode.hpp"
#include "Nodes/ASTIntegerNode.hpp"
#include "Nodes/ASTBinaryOperation.hpp"
#include "Nodes/ASTIdentifierNode.hpp"
#include "Nodes/ASTRootNode.hpp"

#include "../Tokenizer/TokenizerTypes.hpp"

using namespace Shakara;
using namespace Shakara::AST;

void ASTBuilder::Build(
	RootNode* root,
	std::vector<Token>& tokens,
	size_t index
)
{
	const Token& token = tokens[index];
	ptrdiff_t next     = -1;

	// Check if there is anything to do with
	// an identifier first, as most portions
	// of the language deal with identifiers
	if (token.type == TokenType::IDENTIFIER)
	{
		// Check if there's an equal sign
		// after this identifier, and if
		// so, it's an assignment
		if (tokens[index + 1].type == TokenType::EQUAL)
			_ParseAssignment(
				root,
				tokens,
				index,
				&next
			);
	}

	if (next > 0 && static_cast<size_t>(next) < tokens.size())
		Build(
			root,
			tokens,
			next
		);
}

void ASTBuilder::_ParseAssignment(
	RootNode*           root,
	std::vector<Token>& tokens,
	size_t              index,
	ptrdiff_t*			next
)
{
	// Start by setting the next token
	// as the index passed in
	*next = index;

	// Create the regular nodes to be added
	// to the 
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
	// There is no other operation to do, now
	// check if this type is a identifier or
	// a number of sorts
	else if (
		tokens[*next].type == TokenType::IDENTIFIER ||
		tokens[*next].type == TokenType::INTEGER ||
		tokens[*next].type == TokenType::DECIMAL
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

void ASTBuilder::_ParseBinaryOperation(
	BinaryOperation*    operation,
	std::vector<Token>& tokens,
	size_t              index,
	ptrdiff_t*          next
)
{
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
	NodeType         op   = NodeType::ROOT;

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
			tokens[index]
		);

		rightHand->Parent(operation);

		operation->RightHand(rightHand);
	}

	(*next)++;
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
	// TODO: Implement DECIMAL
	default:
		break;
	}
}