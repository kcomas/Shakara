#pragma once

#include "../ASTNode.hpp"

namespace Shakara
{
	namespace AST
	{
		class IdentifierNode : public Node
		{
		public:
			IdentifierNode() = default;

			IdentifierNode(const IdentifierNode& identifier)
			{
				m_type  = identifier.Type();
				m_value = identifier.Value();
			}

			inline IdentifierNode& Value(const std::string& value)
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
				return new IdentifierNode(*this);
			}

		private:
			std::string m_value = "";

		};
	}
}