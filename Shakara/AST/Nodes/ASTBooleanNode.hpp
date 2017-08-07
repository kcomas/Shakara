#pragma once

#include "../ASTNode.hpp"

namespace Shakara
{
	namespace AST
	{
		class BooleanNode : public Node
		{
		public:
			BooleanNode() = default;

			BooleanNode(const BooleanNode& rhs)
			{
				m_value  = rhs.Value();
				m_type   = rhs.Type();
				m_parent = rhs.Parent();
			}

			inline BooleanNode& Value(bool value)
			{
				m_value = value;

				return *this;
			}

			inline bool Value() const
			{
				return m_value;
			}

			Node* Clone()
			{
				return new BooleanNode(*this);
			}

		private:
			bool m_value = false;

		};
	}
}