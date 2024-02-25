#include "Parsers.h"

namespace ReParser::AST
{
    DEFINE_DERIVED_CLASS(RequiredIdentifierNode, ASTNodeParser)
    ASTNodePtr RequiredIdentifierNode::Parse(ICodeFile* file, ASTParser& context, const Token& token)
    {
        return nullptr;
    }

    DEFINE_DERIVED_CLASS(OrNode, ASTNodeParser)
    ASTNodePtr OrNode::Parse(ICodeFile* file, ASTParser& context, const Token& token)
    {
        return nullptr;
    }

    DEFINE_DERIVED_CLASS(GroupNode, ASTNodeParser)
    ASTNodePtr GroupNode::Parse(ICodeFile* file, ASTParser& context, const Token& token)
    {
        return nullptr;
    }

    DEFINE_DERIVED_CLASS(OptionNode, ASTNodeParser)
    ASTNodePtr OptionNode::Parse(ICodeFile* file, ASTParser& context, const Token& token)
    {
        return nullptr;
    }

    DEFINE_DERIVED_CLASS(OptionalRepeatNode, ASTNodeParser)
    ASTNodePtr OptionalRepeatNode::Parse(ICodeFile* file, ASTParser& context, const Token& token)
    {
        return nullptr;
    }

    DEFINE_DERIVED_CLASS(RepeatNode, ASTNodeParser)
    ASTNodePtr RepeatNode::Parse(ICodeFile* file, ASTParser& context, const Token& token)
    {
        return nullptr;
    }
}