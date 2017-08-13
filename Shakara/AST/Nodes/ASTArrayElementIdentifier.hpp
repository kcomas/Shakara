#pragma once

#include "../ASTNode.hpp"

namespace Shakara
{
	namespace AST
	{
		class ArrayElementIdentifierNode : public Node
		{
		public:
			ArrayElementIdentifierNode() = default;

			ArrayElementIdentifierNode(const ArrayElementIdentifierNode& identifier)
			{
				m_type            = identifier.Type();
				m_arrayIdentifier = identifier.ArrayIdentifier();
				m_index           = identifier.Index()->Clone();
			}

			~ArrayElementIdentifierNode()
			{
				delete m_index;
			}

			inline ArrayElementIdentifierNode& ArrayIdentifier(const std::string& identifier)
			{
				m_arrayIdentifier = identifier;

				return *this;
			}

			inline const std::string& ArrayIdentifier() const
			{
				return m_arrayIdentifier;
			}

			inline ArrayElementIdentifierNode& Index(Node* index)
			{
				index->Parent(this);

				m_index = index;

				return *this;
			}

			inline Node* Index() const
			{
				return m_index;
			}

			Node* Clone()
			{
				return new ArrayElementIdentifierNode(*this);
			}

		private:
			std::string m_arrayIdentifier = "";

			Node*       m_index           = nullptr;

		};
	}
}