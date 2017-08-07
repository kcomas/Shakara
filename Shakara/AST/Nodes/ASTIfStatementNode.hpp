#pragma once

#include "../ASTNode.hpp"

namespace Shakara
{
	namespace AST
	{
		class IfStatement : public Node
		{
		public:
			~IfStatement()
			{
				delete m_condition;

				delete m_body;
			}

			inline IfStatement& Condition(Node* node)
			{
				m_condition = node;

				return *this;
			}

			inline IfStatement& Body(Node* node)
			{
				node->Parent(this);

				m_body = node;

				return *this;
			}

			inline Node* Condition()
			{
				return m_condition;
			}

			inline Node* Body()
			{
				return m_body;
			}

		private:
			Node*              m_condition = nullptr;

			Node*              m_body      = nullptr;

		};
	}
}