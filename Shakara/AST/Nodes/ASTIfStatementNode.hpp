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

				if (m_elseIfCondition)
					delete m_elseIfCondition;

				if (m_elseBlock)
					delete m_elseBlock;
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

			inline IfStatement& ElseIfCondition(IfStatement* node)
			{
				node->Parent(this);

				m_elseIfCondition = node;

				return *this;
			}

			inline IfStatement& ElseBlock(Node* node)
			{
				node->Parent(this);

				m_elseBlock = node;

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

			inline IfStatement* ElseIfCondition()
			{
				return m_elseIfCondition;
			}

			inline Node* ElseBlock()
			{
				return m_elseBlock;
			}

		private:
			Node* m_condition              = nullptr;

			Node* m_body                   = nullptr;

			/**
			 * A singular else if condition for the
			 * current if statement, if nullptr,
			 * defaults to else block, which then
			 * won't do anything otherwise
			 */
			IfStatement* m_elseIfCondition = nullptr;

			Node* m_elseBlock              = nullptr;

		};
	}
}