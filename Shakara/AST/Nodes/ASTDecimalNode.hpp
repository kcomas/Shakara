#pragma once

#include "../ASTNode.hpp"

namespace Shakara
{
	namespace AST
	{
		class DecimalNode : public Node
		{
		public:
			DecimalNode() = default;

			DecimalNode(const DecimalNode& rhs)
			{
				m_value  = rhs.Value();
				m_type   = rhs.Type();
				m_parent = rhs.Parent();
			}

			inline DecimalNode& Value(bool negate, float value)
			{
				m_value    = ((negate) ? -value : value);
				m_negative = negate;

				return *this;
			}

			inline float Value() const
			{
				return m_value;
			}

		private:
			bool m_negative = false;

			float m_value = 0;

		};
	}
}