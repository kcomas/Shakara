#pragma once

#include "../ASTNode.hpp"

namespace Shakara
{
	namespace AST
	{
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

		};
	}
}