#pragma once

#include "../ASTNode.hpp"

namespace Shakara
{
	namespace AST
	{
		class AssignmentNode : public Node
		{
		public:
			~AssignmentNode()
			{
				delete m_identifier;
				delete m_assignment;
			}

			inline AssignmentNode& Identifier(Node* node)
			{
				m_identifier = node;

				return *this;
			}

			inline AssignmentNode& Assignment(Node* node)
			{
				m_assignment = node;

				return *this;
			}

			inline Node* GetIdentifier()
			{
				return m_identifier;
			}

			inline Node* GetAssignment()
			{
				return m_assignment;
			}

		private:
			Node* m_identifier = nullptr;

			Node* m_assignment = nullptr;

		};
	}
}