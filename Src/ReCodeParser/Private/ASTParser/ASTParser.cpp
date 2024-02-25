#include "ASTParser.h"
#include "Private/Internal/BaseParser.h"

namespace ReParser::AST
{
    DEFINE_CLASS_WITHOUT_NEW(ASTNode)

    DEFINE_CLASS_WITHOUT_NEW(ASTParser)

    bool Rule::Parse(ASTNodeParser& parser, ICodeFile* file, ASTParser& context, const Token& token)
    {
        if(!Root)
        {
            return false;
        }
        Root = parser.Parse(file, context, token);
        return Root != nullptr;
    }

    bool ASTParser::CompileDeclaration(ICodeFile* file, const Token& token)
    {
        return MainRule.Parse(*Lexer, file, *this, token);
    }
}
