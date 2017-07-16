#pragma once

#include "targetver.hpp"
#include "CppUnitTest.h"

#include <vector>
#include <string>
#include <sstream>

#include "../Shakara/Tokenizer/TokenizerTypes.hpp"
#include "../Shakara/Tokenizer/Tokenizer.hpp"

#include "../Shakara/AST/ASTNode.hpp"
#include "../Shakara/AST/ASTTypes.hpp"
#include "../Shakara/AST/ASTBuilder.hpp"
#include "../Shakara/AST/Nodes/ASTRootNode.hpp"
#include "../Shakara/AST/Nodes/ASTAssignmentNode.hpp"
#include "../Shakara/AST/Nodes/ASTBinaryOperation.hpp"
#include "../Shakara/AST/Nodes/ASTFunctionDeclarationNode.hpp"