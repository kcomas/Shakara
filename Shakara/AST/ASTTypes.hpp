#pragma once

namespace Shakara
{
	namespace AST
	{
		enum class NodeType : uint8_t
		{
			ROOT                     = 0x00,
			ASSIGN                   = 0x01,

			BINARY_OP                = 0x02,
			LOGICAL_OP               = 0x03,
			FUNCTION                 = 0x04,
			CALL                     = 0x05,
			IF_STATEMENT             = 0x06,
			WHILE_STATEMENT          = 0x07,

			/**
			 * Type Nodes
			 */
			INTEGER                  = 0x08,
			DECIMAL                  = 0x09,
			IDENTIFIER               = 0x0A,
			ARRAY_ELEMENT_IDENTIFIER = 0x0B,
			STRING                   = 0x0C,
			BOOLEAN                  = 0x0D,
			ARRAY                    = 0x0E,

			/**
			 * Arithmetic Types for
			 * binary operations
			 */
			ADD                      = 0x0F,
			SUBTRACT                 = 0x10,
			MULTIPLY                 = 0x11,
			DIVIDE                   = 0x12,
			MODULUS                  = 0x13,

			/**
			 * Logical Types
			 */
			EQUAL_COMPARISON         = 0x14,
			NOTEQUAL_COMPARISON      = 0x15,
			LESS_COMPARISON          = 0x16,
			GREATER_COMPARISON       = 0x17,
			LESSEQUAL_COMPARISON     = 0x18,
			GREATEREQUAL_COMPARISON  = 0x19,
			AND                      = 0x1A,
			OR                       = 0x1B,
		
			/**
			 * Special Node Types (return, etc)
			 */
			RETURN                   = 0x1C
		};

		static inline bool IsLogicalOperation(const NodeType& type)
		{
			return (type == NodeType::EQUAL_COMPARISON)        ||
				   (type == NodeType::NOTEQUAL_COMPARISON)     ||
				   (type == NodeType::LESS_COMPARISON)         ||
				   (type == NodeType::GREATER_COMPARISON)      ||
				   (type == NodeType::LESSEQUAL_COMPARISON)    ||
				   (type == NodeType::GREATEREQUAL_COMPARISON) ||
				   (type == NodeType::AND)                     ||
				   (type == NodeType::OR);
		}

		static inline const char* GetNodeTypeName(const NodeType& type)
		{
			switch (type)
			{
			case NodeType::ROOT:
				return "Root";
			case NodeType::ASSIGN:
				return "Assign";
			case NodeType::BINARY_OP:
				return "Binary Operation";
			case NodeType::FUNCTION:
				return "Function Declaration";
			case NodeType::CALL:
				return "Function Call";
			case NodeType::INTEGER:
				return "Integer";
			case NodeType::DECIMAL:
				return "Decimal";
			case NodeType::STRING:
				return "String";
			case NodeType::BOOLEAN:
				return "Boolean";
			case NodeType::IDENTIFIER:
				return "Identifier";
			case NodeType::ADD:
				return "Add";
			case NodeType::SUBTRACT:
				return "Subtract";
			case NodeType::MULTIPLY:
				return "Multiply";
			case NodeType::DIVIDE:
				return "Divide";
			case NodeType::MODULUS:
				return "Modulus";
			case NodeType::RETURN:
				return "Return";
			case NodeType::ARRAY:
				return "Array";
			}

			return "Unknown";
		}
	}
}