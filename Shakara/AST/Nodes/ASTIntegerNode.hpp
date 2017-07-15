#pragma once

#include "../ASTNode.hpp"

namespace Shakara
{
	namespace AST
	{
		class IntegerNode : public Node
		{
		public:
			inline IntegerNode& Value(bool negate, int32_t value)
			{
				m_value = ((negate) ? -value : value);

				return *this;
			}

			inline int32_t Value()
			{
				return m_value;
			}

		private:
			bool negative   = false;

			int32_t m_value = 0;

		};
	}
}