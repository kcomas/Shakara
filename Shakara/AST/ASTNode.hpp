#pragma once

namespace Shakara
{
	namespace AST
	{
		enum class NodeType : uint8_t;

		/**
		 * A singular node in the AST
		 *
		 * Can have a name, children,
		 * type, and a string value.
		 */
		class Node
		{
		public:
			virtual ~Node()
			{
			}

			inline Node& Type(const NodeType& type)
			{
				m_type = type;

				return *this;
			}

			inline Node& Parent(Node* parent)
			{
				m_parent = parent;
				
				return *this;
			}

			inline Node* Parent() const
			{
				return m_parent;
			}

			inline NodeType Type() const
			{
				return m_type;
			}

		protected:
			Node*             m_parent = nullptr;
			
			NodeType          m_type;

		};
	}
}