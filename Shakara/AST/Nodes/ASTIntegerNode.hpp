#pragma once

#include "../ASTNode.hpp"

namespace Shakara
{
	namespace AST
	{
		class IntegerNode : public Node
		{
		public:
			IntegerNode() = default;

			IntegerNode(const IntegerNode& rhs)
			{
				m_value  = rhs.Value();
				m_type   = rhs.Type();
				m_parent = rhs.Parent();
			}

			inline IntegerNode& Value(bool negate, int32_t value)
			{
				m_value    = ((negate) ? -value : value);
				m_negative = negate;

				return *this;
			}

			inline int32_t Value() const
			{
				return m_value;
			}

			Node* Clone()
			{
				return new IntegerNode(*this);
			}

		private:
			bool m_negative   = false;

			int32_t m_value = 0;

		};
	}
}