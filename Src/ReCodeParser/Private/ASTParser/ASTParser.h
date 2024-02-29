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
    };
    using ASTNodePtr = Re::SharedPtr<ASTNode>;

    template<typename T, class ... Ts>
    ASTNodePtr CreateASTNode(Ts&& ... args)
    {
        return Re::MakeShared<T>(std::forward<Ts>(args)...);
    }

    class ASTNodeParser
    {
        DECLARE_CLASS(ASTNodeParser)
    public:
        virtual ~ASTNodeParser() = default;
        virtual ASTNodePtr Parse(ICodeFile* file, ASTParser& context, const Token& token) = 0;
    };

    class Rule
    {
    public:
        bool Parse(ASTNodeParser& parser, ICodeFile* file, ASTParser& context, const Token& token);
        Re::SharedPtr<ASTNode> Root;
    };

    class ASTParser : public BaseParserWithFile
    {
        DECLARE_CLASS(ASTParser)
    public:
        ASTParser(Re::UniquePtr<ASTNodeParser>&& lexer)
            : Lexer(RE_MOVE(lexer))
        {
        }

        bool CompileDeclaration(ICodeFile* file, const Token& token) override;
    private:
        Rule MainRule;
        Re::UniquePtr<ASTNodeParser> Lexer;
    };
}
