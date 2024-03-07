#include "BNFParser.h"
#include "ReClassMisc.h"
#include "ASTParser/Parsers.h"

namespace ReParser::BNF
{
    class BNFParser : public BaseParserWithFile
    {
        enum class ParseState
        {
            Global,
            Left,
            Right
        };

        bool CompileDeclaration(ICodeFile* file, const Token& token) override;
    private:

        bool ParseGlobal(BNFFile& file, const Token& token);
        bool ParseLeft(BNFFile& file, const Token& token);
        bool ParseRight(BNFFile& file, const Token& token);
        bool ParseASTParserGroup(BNFFile& file, const Token& token, Re::SharedPtr<AST::GroupNodeParser>& outParser);
        bool ParseASTParser(BNFFile& file, const Token& token, Re::SharedPtr<AST::ASTNodeParser>* outParser);

    private:

        int32 LastLine = 0;
        ParseState CurrentState = ParseState::Global;
        Re::Stack<Re::WeakPtr<AST::ASTNodeParser>> ParserStack;
    };

    DEFINE_DERIVED_CLASS_WITHOUT_NEW(BNFFile, ICodeFile)

    Re::SharedPtr<BNFFile> BNFFile::Parse(const Re::String& filePath)
    {
        auto result = Re::MakeShared<BNFFile>(filePath);
        if(!result->IsValid())
        {
            return nullptr;
        }
        BNFParser parser;
        parser.InitParserSource(result->GetFilePath(), result->GetContent().c_str());
        parser.Parse(Re::SharedPtrGet(result));
        return result;
    }

    Re::SharedPtr<BNFFile> BNFFile::Parse(const Re::String& filePath, const Re::String& content)
    {
        auto result = Re::MakeShared<BNFFile>(filePath, content);
        if(!result->IsValid())
        {
            return nullptr;
        }
        BNFParser parser;
        parser.InitParserSource(result->GetFilePath(), result->GetContent().c_str());
        parser.Parse(Re::SharedPtrGet(result));
        return result;
    }

    bool BNFFile::AppendRule(const Re::String& ruleName, Re::SharedPtr<AST::ASTNodeParser>* outParserPtr)
    {
        auto it = RuleLexers.find(ruleName);
        if(it == RuleLexers.end())
        {
            auto result = RuleLexers.insert(RE_MAKE_PAIR(ruleName, Re::MakeShared<AST::GroupNodeParser>()));
            auto parser = result.first->second;
            parser->SetDefinedName(ruleName);
            *outParserPtr = parser;
            return true;
        }
        else
        {
            Re::SharedPtr<AST::GroupNodeParser> groupParser = Re::SharedPtrCast<AST::GroupNodeParser>(it->second);
            if(!groupParser->GetSubRules().empty())
            {
                return false;
            }
            *outParserPtr = groupParser;
            return true;
        }
    }

    Re::String BNFFile::ToString() const
    {
        Re::String Result;
        bool isFirst = true;
        for (auto& lexer : RuleLexers)
        {
            if(!isFirst)
            {
                Result += "\n";
            }
            Result += lexer.first;
            Result += "\t\t::= ";
            Result += lexer.second->ToString();
            isFirst = false;
        }
        return Result;
    }

    bool BNFParser::CompileDeclaration(ICodeFile* file, const Token& token)
    {
        if(!file)
        {
            return false;
        }

        auto bnfFile = ReClassSystem::CastTo<BNFFile>(file);
        if(!bnfFile)
        {
            return false;
        }

        auto Result = false;
        if(CurrentState == ParseState::Global)
        {
            return ParseGlobal(*bnfFile, token);
        }
        else if(CurrentState == ParseState::Left)
        {
            return ParseLeft(*bnfFile, token);
        }
        else if(CurrentState == ParseState::Right)
        {
            return ParseRight(*bnfFile, token);
        }
        else
        {
            SetError(RE_FORMAT("Unknown BNF State, failed to parse !! %s", GetLocation().c_str()));
            return false;
        }

    }

    bool BNFParser::ParseGlobal(BNFFile& file, const Token& token)
    {
        if(!token.Matches('<'))
        {
            SetError(RE_FORMAT("BNF line should start with '<' %s", GetFileLocation(&file).c_str()));
            return false;
        }
        UngetToken(token);
        CurrentState = ParseState::Left;
        return true;
    }

    bool BNFParser::ParseLeft(BNFFile& file, const Token& token)
    {
        if(!token.Matches('<'))
        {
            SetError(RE_FORMAT("BNF line should start with '<'", GetFileLocation(&file).c_str()));
            return false;
        }
        Re::String lexerName;
        Token currentToken = token;
        while(true)
        {
            currentToken = *GetToken(false);
            if(currentToken.Matches('>'))
            {
                break;
            }
            lexerName += currentToken.GetTokenName();
        }

        if(lexerName.empty())
        {
            SetError(RE_FORMAT("BNF rule name cannot be null %s", GetFileLocation(&file).c_str()));
            return false;
        }

        Re::SharedPtr<AST::ASTNodeParser> rootParser;
        if(!file.AppendRule(lexerName, &rootParser))
        {
            SetError(RE_FORMAT("BNF rule name %s repeated !! %s", lexerName.c_str(), GetFileLocation(&file).c_str()));
            return false;
        }

        ParserStack.push(rootParser);
        CurrentState = ParseState::Right;

        return true;
    }

    bool BNFParser::ParseRight(BNFFile& file, const Token& token)
    {
        auto currentLine = InputLine;
        if(!(token.Matches("::") && MatchSymbol("=")))
        {
            SetError(RE_FORMAT("BNF rule must split by '::=' operator %s", GetFileLocation(&file).c_str()));
            return false;
        }

        RE_ASSERT(!ParserStack.empty())
        auto root = Re::SharedPtrCast<AST::GroupNodeParser>(ParserStack.top().lock());
        RE_ASSERT(root != nullptr)

        if(IsEndOfLine(currentLine))
        {
            SetError(RE_FORMAT("unexpect EOL %s", GetFileLocation(&file).c_str()));
            return false;
        }
        auto nextToken = GetToken();
        if(!nextToken)
        {
            SetError(RE_FORMAT("unexpect EOL %s", GetFileLocation(&file).c_str()));
            return false;
        }

        ParseASTParserGroup(file, *nextToken, root);

        ParserStack.pop();
        CurrentState = ParseState::Global;

        return true;
    }

    bool BNFParser::ParseASTParserGroup(BNFFile& file, const Token& token, Re::SharedPtr<AST::GroupNodeParser>& outParser)
    {
        if(outParser && !outParser->GetSubRules().empty())
        {
            SetError(RE_FORMAT("group node must be empty before parse !! %s", GetFileLocation(&file).c_str()));
            return false;
        }

        Re::SharedPtr<AST::GroupNodeParser> root = AST::CreateASTNode<AST::GroupNodeParser>();
        Re::SharedPtr<AST::GroupNodeParser> childGroupInOrGroup;
        Re::SharedPtr<AST::OrNodeParser> orGroup;
        auto currentLine = InputLine;
        auto currentToken = token;
        while(true)
        {
            Re::SharedPtr<AST::ASTNodeParser> parser;
            if(ParseASTParser(file, currentToken, &parser))
            {
                if(!orGroup)
                {
                    // normal group (A B)
                    root->AddRule(parser);
                }
                else
                {
                    // sub group in or group ((A B) | (C D))
                    childGroupInOrGroup->AddRule(parser);
                }
            }
            else
            {
                SetError(RE_FORMAT("parse BNF rule failed %s", GetFileLocation(&file).c_str()));
                return false;
            }

            if(IsEndOfLine(currentLine))
            {
                break;
            }

            if(MatchSymbol("|"))
            {
                if(!orGroup)
                {
                    // change normal group to or group (xxx) -> ((xxx)|(xxx))
                    orGroup = AST::CreateASTNode<AST::OrNodeParser>();
                    orGroup->AddRule(root);
                    root = AST::CreateASTNode<AST::GroupNodeParser>();
                    root->AddRule(orGroup);
                    // add next values in child groups
                    childGroupInOrGroup = AST::CreateASTNode<AST::GroupNodeParser>();
                }
                else
                {
                    // add child group to or group, and create new child group
                    orGroup->AddRule(childGroupInOrGroup);
                    childGroupInOrGroup = AST::CreateASTNode<AST::GroupNodeParser>();
                }
            }

            if(IsEndOfLine(currentLine))
            {
                break;
            }
            currentToken = *GetToken();
        }
        if(!outParser)
        {
            outParser = root;
        }
        else
        {
            for (auto subRule : root->GetSubRules())
            {
                outParser->AddRule(subRule);
            }
        }
        return true;
    }

    bool BNFParser::ParseASTParser(BNFFile& file, const Token& token, Re::SharedPtr<AST::ASTNodeParser>* outParser)
    {
        auto currentLine = InputLine;
        Re::SharedPtr<AST::ASTNodeParser> result;
        if(token.Matches("<"))
        {
            // handle <xxx>
            Re::String lexerName;
            Token currentToken = token;
            while(true)
            {
                currentToken = *GetToken(false);
                if(currentToken.Matches('>'))
                {
                    break;
                }
                if(currentLine != InputLine)
                {
                    SetError(RE_FORMAT("unexpect EOL %s", GetFileLocation(&file).c_str()));
                    return false;
                }
                lexerName += currentToken.GetTokenName();
            }
            auto it = file.GetRuleLexers().find(lexerName);
            if(it == file.GetRuleLexers().end())
            {
                file.AppendRule(lexerName, &result);
            }
        }
        else if(token.Matches("["))
        {
            // handle [xxx]
            // parse as [(xxx)]
            auto group = AST::CreateASTNode<AST::GroupNodeParser>();
            auto optionalNode = AST::CreateASTNode<AST::OptionNodeParser>(group);
            while(true)
            {
                if(MatchSymbol("]"))
                {
                    break;
                }
                if(currentLine != InputLine)
                {
                    SetError(RE_FORMAT("unexpect EOL %s", GetFileLocation(&file).c_str()));
                    return false;
                }
                auto nextToken = GetToken();
                Re::SharedPtr<AST::ASTNodeParser> nextParser;
                if(!ParseASTParser(file, *nextToken, &nextParser))
                {
                    SetError(RE_FORMAT("parse ASTParser failed !! %s", GetFileLocation(&file).c_str()));
                    return false;
                }
                group->AddRule(nextParser);
            }
            result = optionalNode;
        }
        else if(token.Matches("{"))
        {
            // handle {xxx}
            // parse as [(xxx)]
            auto group = AST::CreateASTNode<AST::GroupNodeParser>();
            auto optionalNode = AST::CreateASTNode<AST::OptionalRepeatNodeParser>(group);
            while(true)
            {
                if(MatchSymbol("}"))
                {
                    break;
                }
                if(currentLine != InputLine)
                {
                    SetError(RE_FORMAT("unexpect EOL %s", GetFileLocation(&file).c_str()));
                    return false;
                }
                auto nextToken = GetToken();
                Re::SharedPtr<AST::ASTNodeParser> nextParser;
                if(!ParseASTParser(file, *nextToken, &nextParser))
                {
                    SetError(RE_FORMAT("parse ASTParser failed !! %s", GetFileLocation(&file).c_str()));
                    return false;
                }
                group->AddRule(nextParser);
            }
            result = optionalNode;
        }
        else if(token.Matches("("))
        {
            // handle (x x x)
            auto group = AST::CreateASTNode<AST::GroupNodeParser>();
            if(!ParseASTParserGroup(file, token, group))
            {
                SetError(RE_FORMAT("parse group node failed !! %s", GetFileLocation(&file).c_str()));
                return false;
            }
            result = group;
        }
        else if(token.GetTokenType() == ETokenType::Const && token.ConstType == ETokenConstType::String)
        {
            // handle "xxx"
            result = AST::CreateASTNode<AST::RequiredIdentifierNodeParser>(token.GetConstantValue());
        }
        else
        {
            SetError(RE_FORMAT("invalid rule info %s", GetFileLocation(&file).c_str()));
            return false;
        }

        auto afterToken = GetToken(true);
        if(currentLine == InputLine)
        {
            if(afterToken)
            {
                if(afterToken->Matches("*"))
                {
                    // handle
                    auto repeatNode = AST::CreateASTNode<AST::OptionalRepeatNodeParser>(result);
                    result = repeatNode;
                }
                else if(afterToken->Matches("+"))
                {
                    auto repeatNode = AST::CreateASTNode<AST::RepeatNodeParser>(result);
                    result = repeatNode;
                }
                else
                {
                    UngetToken(afterToken);
                }
            }
        }
        else
        {
            UngetToken(afterToken);
        }

        *outParser = result;
        return true;
    }

}
