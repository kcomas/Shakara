#pragma once

#include "../ASTNode.hpp"

namespace Shakara
{
	namespace AST
	{
		/**
		 * A node to be used as a dynamic or static array
		 * type.
		 */
		class ArrayNode : public Node
		{
		public:
			~ArrayNode()
			{
				delete m_capacity;

				for (size_t index = 0; index < m_elements.size(); index++)
					delete m_elements[index];

				m_elements.clear();
			}

			inline ArrayNode& Insert(Node* node)
			{
				node->Parent(this);

				m_elements.push_back(node);

				return *this;
			}

			inline ArrayNode& Fixed(bool state)
			{
				m_fixed = state;

				return *this;
			}

			inline bool Fixed() const
			{
				return m_fixed;
			}

			inline size_t Size() const
			{
				return m_elements.size();
			}

			inline ArrayNode& Capacity(Node* value)
			{
				m_capacity = value;

				return *this;
			}

			inline Node* Capacity() const
			{
				return m_capacity;
			}

			inline Node* operator[](const size_t index) const
			{
				return m_elements[index];
			}

		private:
			bool m_fixed = false;

			Node* m_capacity = nullptr;
			
			std::vector<Node*> m_elements;

		};
	}
}