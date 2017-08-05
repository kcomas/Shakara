#pragma once

#include "../ASTNode.hpp"

namespace Shakara
{
	namespace AST
	{
		enum class CallFlags : uint8_t {
			NONE  = 0x00,
			PRINT = 0x01
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