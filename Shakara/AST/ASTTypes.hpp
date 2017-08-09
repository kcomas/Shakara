#pragma once

namespace Shakara
{
	namespace AST
	{
		enum class NodeType : uint8_t
		{
			ROOT                = 0x00,
			ASSIGN              = 0x01,

			BINARY_OP           = 0x02,
			LOGICAL_OP          = 0x03,
			FUNCTION            = 0x04,
			CALL                = 0x05,
			IF_STATEMENT        = 0x06,
			WHILE_STATEMENT     = 0x07,

			/**
			 * Type Nodes
			 */
			INTEGER             = 0x08,
			DECIMAL             = 0x09,
			IDENTIFIER          = 0x0A,
			STRING              = 0x0B,
			BOOLEAN             = 0x0C,

			/**
			 * Arithmetic Types for
			 * binary operations
			 */
			ADD                 = 0x0D,
			SUBTRACT            = 0x0E,
			MULTIPLY            = 0x0F,
			DIVIDE              = 0x10,
			MODULUS             = 0x11,

			/**
			 * Logical Types
			 */
			EQUAL_COMPARISON    = 0x12,
			NOTEQUAL_COMPARISON = 0x13,
			AND                 = 0x14,
			OR                  = 0x15,
		
			/**
			 * Special Node Types (return, etc)
			 */
			RETURN              = 0x16
		};

		static inline bool _LogicalOperation(const NodeType& type)
		{
			return (type == NodeType::EQUAL_COMPARISON)    ||
				   (type == NodeType::NOTEQUAL_COMPARISON) ||
				   (type == NodeType::AND)                 ||
				   (type == NodeType::OR);
		}

		static inline const char* _GetNodeTypeName(const NodeType& type)
		{
			switch (type)
			{
			case NodeType::ROOT:
				return "ROOT";
			case NodeType::ASSIGN:
				return "ASSIGN";
			case NodeType::BINARY_OP:
				return "BINARY OPERATION";
			case NodeType::FUNCTION:
				return "FUNCTION DECLARATION";
			case NodeType::CALL:
				return "FUNCTION CALL";
			case NodeType::INTEGER:
				return "INTEGER";
			case NodeType::DECIMAL:
				return "DECIMAL";
			case NodeType::IDENTIFIER:
				return "IDENTIFIER";
			case NodeType::STRING:
				return "STRING";
			case NodeType::ADD:
				return "ADD";
			case NodeType::SUBTRACT:
				return "SUBTRACT";
			case NodeType::MULTIPLY:
				return "MULTIPLY";
			case NodeType::BOOLEAN:
				return "BOOLEAN";
			case NodeType::DIVIDE:
				return "DIVIDE";
			case NodeType::RETURN:
				return "RETURN";
			}

			return "UNKNOWN";
		}
	}
}