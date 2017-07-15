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

			/**
			 * Arithmetic Types for
			 * binary operations
			 */
			ADD             = 0x08,
			SUBTRACT        = 0x09,
			MULTIPLY        = 0x0A,
			DIVIDE          = 0x0B,
		};
	}
}