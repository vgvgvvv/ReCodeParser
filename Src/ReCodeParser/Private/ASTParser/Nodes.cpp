#include "Nodes.h"

namespace ReParser::AST
{
    DEFINE_DERIVED_CLASS_WITHOUT_NEW(IdentifierNode, ASTNode)
    DEFINE_DERIVED_CLASS_WITHOUT_NEW(SymbolNode, ASTNode)
    DEFINE_DERIVED_CLASS_WITHOUT_NEW(ConstNode, ASTNode)

    DEFINE_DERIVED_CLASS_WITHOUT_NEW(NumNode, ConstNode)
    DEFINE_DERIVED_CLASS_WITHOUT_NEW(StringNode, ConstNode)

}
