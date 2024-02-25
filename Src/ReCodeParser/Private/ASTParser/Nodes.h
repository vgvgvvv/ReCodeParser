#pragma once
#include "ASTParser.h"

namespace ReParser::AST
{

    // node below is for basic tokens

    // identifier token
    class IdentifierNode : public ASTNode
    {
        DECLARE_DERIVED_CLASS(IdentifierNode, ASTNode)
    public:

    };

    // symbol token
    class SymbolNode : public ASTNode
    {
        DECLARE_DERIVED_CLASS(SymbolNode, ASTNode)
    public:

    };

    // number token
    class NumNode : public ASTNode
    {
        DECLARE_DERIVED_CLASS(NumNode, ASTNode)
    public:

    };

    // string token
    class StringNode : public ASTNode
    {
        DECLARE_DERIVED_CLASS(StringNode, ASTNode)
    public:

    };

    // const value
    class ConstNode : public ASTNode
    {
        DECLARE_DERIVED_CLASS(ConstNode, ASTNode)
    public:

    };

    // block node
    class BlockNode : public ASTNode
    {
    public:

    };


}