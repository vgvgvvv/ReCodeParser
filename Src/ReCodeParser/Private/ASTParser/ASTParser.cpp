#include "ASTParser.h"
#include "Private/Internal/BaseParser.h"

namespace ReParser::AST
{
    DEFINE_CLASS_WITHOUT_NEW(ASTNode)

    DEFINE_CLASS_WITHOUT_NEW(ASTParser)

    bool Rule::Parse(ICodeFile* file, ASTParser& context, const Token& token)
    {
        if(!Root)
        {
            return false;
        }
        return Root->Parse(file, context, token);
    }

    bool ASTParser::CompileDeclaration(ICodeFile* file, const Token& token)
    {
        return MainRule.Parse(file, *this, token);
    }
}
