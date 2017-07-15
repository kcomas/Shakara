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
			~BinaryOperation()
			{
				delete m_leftHand;
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

			inline Node* GetLeftHand()
			{
				return m_leftHand;
			}

			inline NodeType& Operation()
			{
				return m_operation;
			}

			inline Node* GetRightHand()
			{
				return m_rightHand;
			}

		private:
			Node*    m_leftHand  = nullptr;

			NodeType m_operation;

			Node*    m_rightHand = nullptr;

		};
	}
}