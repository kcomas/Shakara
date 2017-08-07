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
			FUNCTION            = 0x03,
			CALL                = 0x04,
			IF_STATEMENT        = 0x05,

			/**
			 * Type Nodes
			 */
			INTEGER             = 0x06,
			DECIMAL             = 0x07,
			IDENTIFIER          = 0x08,
			STRING              = 0x09,
			BOOLEAN             = 0x0A,

			/**
			 * Arithmetic Types for
			 * binary operations
			 */
			ADD                 = 0x0B,
			SUBTRACT            = 0x0C,
			MULTIPLY            = 0x0D,
			DIVIDE              = 0x0E,

			/**
			 * Logical Types
			 */
			EQUAL_COMPARISON    = 0x0F,
			NOTEQUAL_COMPARISON = 0x10,
			AND                 = 0x11,
			OR                  = 0x12,

			/**
			 * Special Node Types (return, etc)
			 */
			RETURN              = 0x13
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