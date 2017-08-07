#pragma once

#include "../ASTNode.hpp"

namespace Shakara
{
	namespace AST
	{
		class StringNode : public Node
		{
		public:
			StringNode() = default;

			StringNode(const StringNode& rhs)
			{
				m_value  = rhs.Value();
				m_type   = rhs.Type();
				m_parent = rhs.Parent();
			}

			inline StringNode& Value(const std::string& value)
			{
				m_value = value;

				return *this;
			}

			inline const std::string& Value() const
			{
				return m_value;
			}

			Node* Clone()
			{
				return new StringNode(*this);
			}

		private:
			std::string m_value = "";

		};
	}
}