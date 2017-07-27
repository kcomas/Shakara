#pragma once

#include "../ASTNode.hpp"

namespace Shakara
{
	namespace AST
	{
		class StringNode : public Node
		{
		public:
			inline StringNode& Value(const std::string& value)
			{
				m_value = value;

				return *this;
			}

			inline const std::string& Value() const
			{
				return m_value;
			}

		private:
			std::string m_value = "";

		};
	}
}