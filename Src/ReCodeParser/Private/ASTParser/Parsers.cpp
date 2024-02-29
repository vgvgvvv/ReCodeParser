#include "Parsers.h"
#include "Nodes.h"

namespace ReParser::AST
{
    DEFINE_DERIVED_CLASS(RequiredIdentifierNode, ASTNodeParser)
    // `xxx` in BNF
    ASTNodePtr RequiredIdentifierNode::Parse(ICodeFile* file, ASTParser& context, const Token& token)
    {
        if(token.Matches(TokenName.c_str()))
        {
            return CreateASTNode<IdentifierNode>(token);
        }
        return nullptr;
    }

    DEFINE_DERIVED_CLASS(OrNode, ASTNodeParser)
    // a | b in BNF
    ASTNodePtr OrNode::Parse(ICodeFile* file, ASTParser& context, const Token& token)
    {
        for (auto& subRule : SubRules)
        {
            auto rule = Re::WeakPtrGetSafe(subRule);
            if(!rule)
            {
                continue;
            }
            auto result = rule->Parse(file, context, token);
            if(result)
            {
               return result;
            }
            context.UngetToken(token);
            auto nextToken = context.GetToken(file);
            RE_ASSERT(*nextToken == token);
        }
        return nullptr;
    }

    DEFINE_DERIVED_CLASS(GroupNode, ASTNodeParser)
    // (a b) in BNF
    ASTNodePtr GroupNode::Parse(ICodeFile* file, ASTParser& context, const Token& token)
    {
        return nullptr;
    }

    DEFINE_DERIVED_CLASS(OptionNode, ASTNodeParser)
    // [a] in BNF
    ASTNodePtr OptionNode::Parse(ICodeFile* file, ASTParser& context, const Token& token)
    {
        return nullptr;
    }

    DEFINE_DERIVED_CLASS(OptionalRepeatNode, ASTNodeParser)

    // {a} or a* in BNF
    ASTNodePtr OptionalRepeatNode::Parse(ICodeFile* file, ASTParser& context, const Token& token)
    {
        return nullptr;
    }

    DEFINE_DERIVED_CLASS(RepeatNode, ASTNodeParser)
    // a+ in BNF
    ASTNodePtr RepeatNode::Parse(ICodeFile* file, ASTParser& context, const Token& token)
    {
        return nullptr;
    }
}