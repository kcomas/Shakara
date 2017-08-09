#pragma once

#include "../ASTNode.hpp"

namespace Shakara
{
	namespace AST
	{
		enum class NodeType : uint8_t;

		class BinaryOperation : public Node
		{
		public:
			BinaryOperation() = default;

			BinaryOperation(const BinaryOperation& rhs)
			{
				m_type      = rhs.Type();
				m_leftHand  = rhs.GetLeftHand()->Clone();
				m_operation = rhs.Operation();
				m_rightHand = rhs.GetRightHand()->Clone();
			}

			~BinaryOperation()
			{
				if (m_leftHand)
					delete m_leftHand;

				if (m_rightHand)
					delete m_rightHand;
			}

			inline BinaryOperation& LeftHand(Node* node)
			{
				m_leftHand = node;

				return *this;
			}

			inline BinaryOperation& Operation(NodeType operation)
			{
				m_operation = operation;

				return *this;
			}

			inline BinaryOperation& RightHand(Node* node)
			{
				m_rightHand = node;

				return *this;
			}

			inline Node* GetLeftHand() const
			{
				return m_leftHand;
			}

			inline NodeType Operation() const
			{
				return m_operation;
			}

			inline Node* GetRightHand() const
			{
				return m_rightHand;
			}

			Node* Clone()
			{
				return new BinaryOperation(*this);
			}

		private:
			Node*    m_leftHand  = nullptr;

			NodeType m_operation;

			Node*    m_rightHand = nullptr;

		};
	}
}