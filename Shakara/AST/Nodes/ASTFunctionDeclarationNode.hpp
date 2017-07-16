#pragma once

#include "../ASTNode.hpp"

namespace Shakara
{
	namespace AST
	{
		class FunctionDeclaration : public Node
		{
		public:
			~FunctionDeclaration()
			{
				for (size_t index = 0; index < m_arguments.size(); index++)
					delete m_arguments[index];

				m_arguments.clear();

				delete m_body;
			}

			inline FunctionDeclaration& Identifier(Node* node)
			{
				m_identifier = node;

				return *this;
			}

			inline FunctionDeclaration& InsertArgument(Node* node)
			{
				node->Parent(this);

				m_arguments.push_back(node);

				return *this;
			}

			inline FunctionDeclaration& Body(Node* node)
			{
				node->Parent(this);

				m_body = node;

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

			inline Node* Body()
			{
				return m_body;
			}

		private:
			Node*              m_identifier = nullptr;

			std::vector<Node*> m_arguments;

			Node*              m_body       = nullptr;

		};
	}
}