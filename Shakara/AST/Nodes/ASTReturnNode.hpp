#pragma once

#include "../ASTNode.hpp"

namespace Shakara
{
	namespace AST
	{
		class ReturnNode : public Node
		{
		public:
			~ReturnNode()
			{
				delete m_returned;
			}

			inline ReturnNode& Returned(Node* node)
			{
				m_returned = node;

				return *this;
			}

			inline Node* GetReturned()
			{
				return m_returned;
			}

		private:
			Node* m_returned = nullptr;

		};
	}
}