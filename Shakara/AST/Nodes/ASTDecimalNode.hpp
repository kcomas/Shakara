#pragma once

#include "../ASTNode.hpp"

namespace Shakara
{
	namespace AST
	{
		class DecimalNode : public Node
		{
		public:
			inline DecimalNode& Value(bool negate, float value)
			{
				m_value = ((negate) ? -value : value);

				return *this;
			}

			inline float Value() const
			{
				return m_value;
			}

		private:
			bool negative = false;

			float m_value = 0;

		};
	}
}