#pragma once

#include "../ASTNode.hpp"

namespace Shakara
{
	namespace AST
	{
		/**
		 * A node to be used as a root for a whole
		 * AST, as well as for things like function
		 * declarations.
		 */
		class RootNode : public Node
		{
		public:
			~RootNode()
			{
				for (size_t index = 0; index < m_children.size(); index++)
					delete m_children[index];

				m_children.clear();
			}

			inline RootNode& Insert(Node* node)
			{
				node->Parent(this);

				m_children.push_back(node);

				return *this;
			}

			inline RootNode& ShrinkToFit()
			{
				m_children.shrink_to_fit();

				return *this;
			}

			inline size_t Children()
			{
				return m_children.size();
			}

			inline Node* operator[](const size_t index) const
			{
				return m_children[index];
			}

		private:
			std::vector<Node*> m_children;

		};
	}
}