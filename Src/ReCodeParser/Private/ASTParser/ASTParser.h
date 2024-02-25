#pragma once
#include "ReClassInfo.h"
#include "Internal/BaseParser.h"

namespace ReParser::AST
{
    class ASTParser;

    class ASTNode
    {
        DECLARE_CLASS(ASTNode)
    public:
        virtual ~ASTNode() = default;
        virtual bool Parse(ICodeFile* file, ASTParser& context, const Token& token) = 0;
    };

    class Rule
    {
    public:
        bool Parse(ICodeFile* file, ASTParser& context, const Token& token);
        Re::UniquePtr<ASTNode> Root;
    };

    class ASTParser : public BaseParserWithFile
    {
        DECLARE_CLASS(ASTParser)
    public:
        bool CompileDeclaration(ICodeFile* file, const Token& token) override;
    private:
        Rule MainRule;
    };
}
