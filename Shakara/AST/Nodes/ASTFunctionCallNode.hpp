#pragma once

#include "../ASTNode.hpp"

namespace Shakara
{
	namespace AST
	{
		enum class CallFlags : uint8_t {
			NONE            = 0x00,
			PRINT           = 0x01,
			TYPE            = 0x02,
			AMOUNT          = 0x03,

			/**
			 * Calls for casting between
			 * built-in types
			 */
			INTEGER_CAST    = 0x04,
			DECIMAL_CAST    = 0x05,
			STRING_CAST     = 0x06,
			BOOLEAN_CAST    = 0x07,

			PUSH_COLLECTION = 0x08,
			POP_COLLECTION  = 0x09
		};

		class FunctionCall : public Node
		{
		public:
			~FunctionCall()
			{
				delete m_identifier;

				for (size_t index = 0; index < m_arguments.size(); index++)
					delete m_arguments[index];

				m_arguments.clear();
			}

			inline FunctionCall& Identifier(Node* node)
			{
				m_identifier = node;

				return *this;
			}

			inline FunctionCall& InsertArgument(Node* node)
			{
				node->Parent(this);

				m_arguments.push_back(node);

				return *this;
			}

			inline void SetFlags(const CallFlags& flags)
			{
				m_flags = flags;
			}

			inline CallFlags Flags()
			{
				return m_flags;
			}

			inline Node* Identifier()
			{
				return m_identifier;
			}

			inline std::vector<Node*>& Arguments()
			{
				return m_arguments;
			}

		private:
			Node*              m_identifier = nullptr;

			std::vector<Node*> m_arguments;

			CallFlags          m_flags      = CallFlags::NONE;

		};
	}
}