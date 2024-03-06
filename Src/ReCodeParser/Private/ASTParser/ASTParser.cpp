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

    bool ASTParser::CompileDeclaration(ICodeFile* file, const Token& token)
    {
        return Tree.Parse(*Lexer, file, *this, token);
    }
}
