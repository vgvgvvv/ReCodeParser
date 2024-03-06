#include "BNFParser.h"
#include "ReClassMisc.h"
#include "ASTParser/Parsers.h"

namespace ReParser::BNF
{
    DEFINE_DERIVED_CLASS_WITHOUT_NEW(BNFFile, ICodeFile)

    Re::SharedPtr<BNFFile> BNFFile::Parse(const Re::String& filePath)
    {
    }

    bool BNFFile::AppendRule(const Re::String& ruleName, AST::ASTNodeParser** outParserPtr)
    {
        auto it = RuleLexers.find(ruleName);
        if(it == RuleLexers.end())
        {
            auto result = RuleLexers.insert(RE_MAKE_PAIR(ruleName, Re::MakeShared<AST::GroupNodeParser>()));
            *outParserPtr = Re::SharedPtrGet(result.first->second);
            return true;
        }
        else
        {
            Re::SharedPtr<AST::GroupNodeParser> groupParser = Re::SharedPtrCast<AST::GroupNodeParser>(it->second);
            if(!groupParser->GetSubRules().empty())
            {
                return false;
            }
            *outParserPtr = Re::SharedPtrGet(groupParser);
            return true;
        }
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
            SetError(RE_FORMAT("BNF line should start with '<'", GetFileLocation(&file).c_str()));
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
            if(token.Matches('>'))
            {
                break;
            }
            currentToken = *GetToken(false);
            lexerName += currentToken.GetTokenName();
        }

        if(lexerName.empty())
        {
            SetError(RE_FORMAT("BNF rule name cannot be null %s", GetFileLocation(&file).c_str()));
            return false;
        }

        AST::ASTNodeParser* rootParser;
        if(!file.AppendRule(lexerName, &rootParser))
        {
            SetError(RE_FORMAT("BNF rule name %s repeated !! %s", lexerName.c_str(), GetFileLocation(&file).c_str()));
            return false;
        }

        ParserStack.push(rootParser);

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
        auto root = ReClassSystem::CastTo<AST::GroupNodeParser>(ParserStack.top());
        RE_ASSERT(root)

        while(true)
        {
            auto nextToken = GetToken();
            if(currentLine != InputLine)
            {
                UngetToken(nextToken);
                break;
            }
            Re::SharedPtr<AST::ASTNodeParser> parser;
            if(ParseASTParser(file, *nextToken, &parser))
            {
                root->AddRule(parser);
            }
            else
            {
                SetError(RE_FORMAT("parse BNF rule failed %s", GetFileLocation(&file).c_str()));
                return false;
            }
        }


        return true;
    }

    bool BNFParser::ParseASTParser(BNFFile& file, const Token& token, Re::SharedPtr<AST::ASTNodeParser>* outParser)
    {
        auto currentLine = InputLine;
        if(token.Matches("<"))
        {
            // handle <xxx>
        }
        else if(token.Matches("["))
        {
            // handle [xxx]
        }
        else if(token.Matches("{"))
        {
            // handle {xxx}
        }
        else if(token.Matches("("))
        {
            // handle (x x x)
        }
        else if(token.GetTokenType() == ETokenType::Const && token.ConstType == ETokenConstType::String)
        {
            // handle "xxx"
        }
        else
        {
            SetError(RE_FORMAT("invalid rule info %s", GetFileLocation(&file).c_str()));
            return false;
        }

        auto afterToken = GetToken(true);
        if(afterToken)
        {
            if(afterToken->Matches("*"))
            {
                // handle
            }
            else if(afterToken->Matches("+"))
            {

            }
            else if(afterToken->Matches("|"))
            {

            }
            else
            {
                SetError(RE_FORMAT("invalid rule info %s", GetFileLocation(&file).c_str()));
                return false;
            }
        }

        return true;
    }

}
