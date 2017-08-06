#pragma once

namespace Shakara
{
	namespace AST
	{
		enum class NodeType : uint8_t
		{
			ROOT            = 0x00,
			ASSIGN          = 0x01,

			BINARY_OP       = 0x02,
			FUNCTION        = 0x03,
			CALL            = 0x04,

			/**
			 * Type Nodes
			 */
			INTEGER         = 0x05,
			DECIMAL         = 0x06,
			IDENTIFIER      = 0x07,
			STRING          = 0x08,

			/**
			 * Arithmetic Types for
			 * binary operations
			 */
			ADD             = 0x09,
			SUBTRACT        = 0x0A,
			MULTIPLY        = 0x0B,
			DIVIDE          = 0x0C,

			/**
			 * Special Node Types (return, etc)
			 */
			RETURN          = 0x0D
		};

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
			case NodeType::DIVIDE:
				return "DIVIDE";
			case NodeType::RETURN:
				return "RETURN";
			}

			return "UNKNOWN";
		}
	}
}