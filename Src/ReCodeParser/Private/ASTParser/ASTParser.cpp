#include "ASTParser.h"
#include "Private/Internal/BaseParser.h"

namespace ReParser::AST
{
    DEFINE_CLASS_WITHOUT_NEW(ASTNode)
    DEFINE_CLASS_WITHOUT_NEW(ASTNodeParser)
    DEFINE_CLASS_WITHOUT_NEW(ASTParser)

    bool ASTTree::Parse(ASTNodeParser& parser, ICodeFile* file, ASTParser& context, const Token& token)
    {
        if(!Root)
        {
            return false;
        }
        return parser.Parse(file, context, token, &Root);
    }

    Re::String ASTTree::ToString() const
    {
        return Root->ToString();
    }

    bool ASTParser::CompileDeclaration(ICodeFile* file, const Token& token)
    {
        return Tree.Parse(*Lexer, file, *this, token);
    }

    void ASTParser::AddCustomParser(const Re::String& name, const Re::SharedPtr<ASTNodeParser>& parser)
    {
        CustomParsers[name] = parser;
    }

    bool ASTParser::TryGetCustomParser(const Re::String& name, Re::SharedPtr<ASTNodeParser>* outParser)
    {
        auto iter = CustomParsers.find(name);
        if(iter != CustomParsers.end())
        {
            *outParser = iter->second;
            return true;
        }
        return false;
    }
}
