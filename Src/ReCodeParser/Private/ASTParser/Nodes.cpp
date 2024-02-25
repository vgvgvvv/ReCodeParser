#include "Nodes.h"

namespace ReParser::AST
{
    DEFINE_DERIVED_CLASS(RequiredIdentifierNode, ASTNode)
    bool RequiredIdentifierNode::Parse(ICodeFile* file, ASTParser& context, const Token& token)
    {
        return true;
    }

    DEFINE_DERIVED_CLASS(OrNode, ASTNode)
    bool OrNode::Parse(ICodeFile* file, ASTParser& context, const Token& token)
    {
        return true;
    }

    DEFINE_DERIVED_CLASS(GroupNode, ASTNode)
    bool GroupNode::Parse(ICodeFile* file, ASTParser& context, const Token& token)
    {
        return true;
    }

    DEFINE_DERIVED_CLASS(OptionNode, ASTNode)
    bool OptionNode::Parse(ICodeFile* file, ASTParser& context, const Token& token)
    {
        return true;
    }

    DEFINE_DERIVED_CLASS(OptionalRepeatNode, ASTNode)
    bool OptionalRepeatNode::Parse(ICodeFile* file, ASTParser& context, const Token& token)
    {
        return true;
    }

    DEFINE_DERIVED_CLASS(RepeatNode, ASTNode)
    bool RepeatNode::Parse(ICodeFile* file, ASTParser& context, const Token& token)
    {
        return true;
    }

    DEFINE_DERIVED_CLASS(IdentifierNode, ASTNode)
    bool IdentifierNode::Parse(ICodeFile* file, ASTParser& context, const Token& token)
    {
        return true;
    }

    DEFINE_DERIVED_CLASS(SymbolNode, ASTNode)
    bool SymbolNode::Parse(ICodeFile* file, ASTParser& context, const Token& token)
    {
        return true;
    }

    DEFINE_DERIVED_CLASS(NumNode, ASTNode)
    bool NumNode::Parse(ICodeFile* file, ASTParser& context, const Token& token)
    {
        return true;
    }

    DEFINE_DERIVED_CLASS(StringNode, ASTNode)
    bool StringNode::Parse(ICodeFile* file, ASTParser& context, const Token& token)
    {
        return true;
    }

    DEFINE_DERIVED_CLASS(ConstNode, ASTNode)
    bool ConstNode::Parse(ICodeFile* file, ASTParser& context, const Token& token)
    {
        return true;
    }
}
