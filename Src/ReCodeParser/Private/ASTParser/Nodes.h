#pragma once
#include "ASTParser.h"

namespace ReParser::AST
{
    // node below is used for BNF

    // 'A' or "A"
    class RequiredIdentifierNode : public ASTNode
    {
        DECLARE_DERIVED_CLASS(RequiredIdentifierNode, ASTNode)
    public:
        bool Parse(ICodeFile* file, ASTParser& context, const Token& token) override;
    };

    // A | B
    class OrNode : public ASTNode
    {
        DECLARE_DERIVED_CLASS(OrNode, ASTNode)
    public:
        bool Parse(ICodeFile* file, ASTParser& context, const Token& token) override;
    };

    // (A B)
    class GroupNode : public ASTNode
    {
        DECLARE_DERIVED_CLASS(GroupNode, ASTNode)
    public:
        bool Parse(ICodeFile* file, ASTParser& context, const Token& token) override;
    };

    // [A] is optional
    class OptionNode : public ASTNode
    {
        DECLARE_DERIVED_CLASS(OptionNode, ASTNode)
    public:
        bool Parse(ICodeFile* file, ASTParser& context, const Token& token) override;
    };

    // {A} or A*
    class OptionalRepeatNode : public ASTNode
    {
        DECLARE_DERIVED_CLASS(OptionalRepeatNode, ASTNode)
    public:
        bool Parse(ICodeFile* file, ASTParser& context, const Token& token) override;
    };

    // A+
    class RepeatNode : public ASTNode
    {
        DECLARE_DERIVED_CLASS(RepeatNode, ASTNode)
    public:
        bool Parse(ICodeFile* file, ASTParser& context, const Token& token) override;
    };

    // node below is for basic tokens

    // identifier token
    class IdentifierNode : public ASTNode
    {
        DECLARE_DERIVED_CLASS(IdentifierNode, ASTNode)
    public:
        bool Parse(ICodeFile* file, ASTParser& context, const Token& token) override;
    };

    // symbol token
    class SymbolNode : public ASTNode
    {
        DECLARE_DERIVED_CLASS(SymbolNode, ASTNode)
    public:
        bool Parse(ICodeFile* file, ASTParser& context, const Token& token) override;
    };

    // number token
    class NumNode : public ASTNode
    {
        DECLARE_DERIVED_CLASS(NumNode, ASTNode)
    public:
        bool Parse(ICodeFile* file, ASTParser& context, const Token& token) override;
    };

    // string token
    class StringNode : public ASTNode
    {
        DECLARE_DERIVED_CLASS(StringNode, ASTNode)
    public:
        bool Parse(ICodeFile* file, ASTParser& context, const Token& token) override;
    };

    class ConstNode : public ASTNode
    {
        DECLARE_DERIVED_CLASS(ConstNode, ASTNode)
    public:
        bool Parse(ICodeFile* file, ASTParser& context, const Token& token) override;
    };


}