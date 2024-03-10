#include "Parsers.h"
#include "ASTParser/Nodes.h"

namespace ReParser::AST
{
    DEFINE_DERIVED_CLASS_WITHOUT_NEW(RequiredIdentifierNodeParser, ASTNodeParser)
    // `xxx` in BNF
    bool RequiredIdentifierNodeParser::Parse(ICodeFile* file, ASTParser& context, const Token& token, ASTNodePtr* outNode)
    {
        if(token.Matches(TokenName.c_str()))
        {
            *outNode = CreateASTNode<IdentifierNode>(token);
            return true;
        }
        return false;
    }

    Re::String RequiredIdentifierNodeParser::ToString() const
    {
        Re::String Result;
        Result += "\"";
        Result += TokenName;
        Result += "\"";
        return Result;
    }

    DEFINE_DERIVED_CLASS_WITHOUT_NEW(OrNodeParser, ASTNodeParser)
    // a | b in BNF
    bool OrNodeParser::Parse(ICodeFile* file, ASTParser& context, const Token& token, ASTNodePtr* outNode)
    {
        for (auto& subRule : SubRules)
        {
            auto rule = Re::SharedPtrGet(subRule);
            if(!rule)
            {
                continue;
            }
            if(rule->Parse(file, context, token, outNode))
            {
               return true;
            }
            context.UngetToken(token);
            auto nextToken = context.GetToken(file);
            RE_ASSERT(*nextToken == token);
        }
        return false;
    }

    Re::String OrNodeParser::ToString() const
    {
        Re::String Result;
        bool isFirst = true;
        for (auto& subRule : SubRules)
        {
            if(!isFirst)
            {
                Result += " | ";
            }
            if(subRule->IsDefinedParser())
            {
                Result += "<";
                Result += subRule->GetName();
                Result += ">";
            }
            else
            {
                Result += subRule->ToString();
            }
            isFirst = false;
        }

        return Result;
    }

    DEFINE_DERIVED_CLASS_WITHOUT_NEW(GroupNodeParser, ASTNodeParser)
    // (a b) in BNF
    bool GroupNodeParser::Parse(ICodeFile* file, ASTParser& context, const Token& token, ASTNodePtr* outNode)
    {
        Re::SharedPtr<GroupNode> result = Re::MakeShared<GroupNode>();
        Token currentToken = token;
        for (auto& subRule : SubRules)
        {
            auto rule = Re::SharedPtrGet(subRule);
            if(!rule)
            {
                return false;
            }
            ASTNodePtr subNode;
            if(!rule->Parse(file, context, currentToken, &subNode))
            {
                context.UngetToken(currentToken);
                auto nextToken = context.GetToken(file);
                RE_ASSERT(*nextToken == currentToken);
                return false;
            }
            else
            {
                result->AppendNode(subNode);
                auto nextToken = context.GetToken();
                currentToken = *nextToken;
            }
        }

        return true;
    }

    Re::String GroupNodeParser::ToString() const
    {
        Re::String Result;
        if(SubRules.size() != 1 && !IsDefinedParser())
        {
            Result += "( ";
        }
        bool isFirst = true;
        for (auto& subRule : SubRules)
        {
            if(!isFirst)
            {
                Result += " ";
            }
            if(subRule->IsDefinedParser())
            {
                Result += "<";
                Result += subRule->GetName();
                Result += ">";
            }
            else
            {
                Result += subRule->ToString();
            }
            isFirst = false;
        }
        if(SubRules.size() != 1 && !IsDefinedParser())
        {
            Result += " )";
        }
        return Result;
    }

    DEFINE_DERIVED_CLASS_WITHOUT_NEW(CustomNodeParser, ASTNodeParser)
    bool CustomNodeParser::Parse(ICodeFile* file, ASTParser& context, const Token& token, ASTNodePtr* outNode)
    {
        if(!RealParser)
        {
            if(!context.TryGetCustomParser(CustomParserName, &RealParser))
            {
                context.SetError(RE_FORMAT("cannot find custom parser %s %s", CustomParserName.c_str(), context.GetFileLocation(file).c_str()));
                return false;
            }
        }
        return RealParser->Parse(file, context, token, outNode);
    }

    DEFINE_DERIVED_CLASS_WITHOUT_NEW(OptionNodeParser, ASTNodeParser)
    // [a] in BNF
    bool OptionNodeParser::Parse(ICodeFile* file, ASTParser& context, const Token& token, ASTNodePtr* outNode)
    {
        if(!SubRule)
        {
            return true;
        }
        if(!SubRule->Parse(file, context, token, outNode))
        {
            context.UngetToken(token);
            auto nextToken = context.GetToken(file);
            RE_ASSERT(*nextToken == token);
            return true;
        }
        return true;
    }

    Re::String OptionNodeParser::ToString() const
    {
        Re::String Result;
        Result += "[";
        if(SubRule->IsDefinedParser())
        {
            Result += "<";
            Result += SubRule->GetName();
            Result += ">";
        }
        else
        {
            Result += SubRule->ToString();
        }
        Result += "]";
        return Result;
    }

    DEFINE_DERIVED_CLASS_WITHOUT_NEW(OptionalRepeatNodeParser, ASTNodeParser)

    // {a} or a* in BNF
    bool OptionalRepeatNodeParser::Parse(ICodeFile* file, ASTParser& context, const Token& token, ASTNodePtr* outNode)
    {
        Re::SharedPtr<GroupNode> result = Re::MakeShared<GroupNode>();
        *outNode = result;
        auto startToken = token;
        while(true)
        {
            ASTNodePtr subNode;
            if(SubRule->Parse(file, context, startToken, &subNode))
            {
                result->AppendNode(subNode);
                startToken = *context.GetToken();
            }
            else
            {
                context.UngetToken(startToken);
                auto nextToken = context.GetToken(file);
                RE_ASSERT(*nextToken == token);
                break;
            }
        }
        return true;
    }

    Re::String OptionalRepeatNodeParser::ToString() const
    {
        Re::String Result;
        Result += "{";
        if(SubRule->IsDefinedParser())
        {
            Result += "<";
            Result += SubRule->GetName();
            Result += ">";
        }
        else
        {
            Result += SubRule->ToString();
        }
        Result += "}";
        return Result;
    }

    DEFINE_DERIVED_CLASS_WITHOUT_NEW(RepeatNodeParser, ASTNodeParser)
    // a+ in BNF
    bool RepeatNodeParser::Parse(ICodeFile* file, ASTParser& context, const Token& token, ASTNodePtr* outNode)
    {
        Re::SharedPtr<GroupNode> result = Re::MakeShared<GroupNode>();
        *outNode = result;
        auto startToken = token;
        while(true)
        {
            ASTNodePtr subNode;
            if(SubRule->Parse(file, context, startToken, &subNode))
            {
                result->AppendNode(subNode);
                startToken = *context.GetToken();
            }
            else
            {
                context.UngetToken(startToken);
                auto nextToken = context.GetToken(file);
                RE_ASSERT(*nextToken == token);
                break;
            }
        }
        if(result->GetSubNodes().empty())
        {
            outNode->reset();
            return false;
        }
        return true;
    }

    Re::String RepeatNodeParser::ToString() const
    {
        Re::String Result;

        if(SubRule->IsDefinedParser())
        {
            Result += "<";
            Result += SubRule->GetName();
            Result += ">";
        }
        else
        {
            Result += SubRule->ToString();
        }
        Result += "+";

        return Result;
    }
}
