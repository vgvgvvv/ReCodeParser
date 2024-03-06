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
        auto it = RuleLexers.insert(RE_MAKE_PAIR(ruleName, Re::MakeShared<AST::GroupNodeParser>()));
        if(!it.second)
        {
            return false;
        }
        *outParserPtr = Re::SharedPtrGet(it.first->second);
        return true;
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
        // TODO parse right
        return true;
    }
}
