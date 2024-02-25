#include "Nodes.h"

namespace ReParser::AST
{
    DEFINE_DERIVED_CLASS(IdentifierNode, ASTNode)
    DEFINE_DERIVED_CLASS(SymbolNode, ASTNode)
    DEFINE_DERIVED_CLASS(NumNode, ASTNode)
    DEFINE_DERIVED_CLASS(StringNode, ASTNode)
    DEFINE_DERIVED_CLASS(ConstNode, ASTNode)

}
