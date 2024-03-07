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
    private:
        Re::String Name;
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
        virtual bool Parse(ICodeFile* file, ASTParser& context, const Token& token, ASTNodePtr* outNode) = 0;
        virtual Re::String ToString() const = 0;

        void SetDefinedName(const Re::String& name)
        {
            CustomName = name;
        }

        bool IsDefinedParser() const
        {
            return !CustomName.empty();
        }

        const char* GetName() const
        {
            if(!CustomName.empty())
            {
                return CustomName.c_str();
            }
            return GetClass().GetName();
        }

    private:
        Re::String CustomName;
    };

    class ASTTree
    {
    public:
        bool Parse(ASTNodeParser& parser, ICodeFile* file, ASTParser& context, const Token& token);
        Re::SharedPtr<ASTNode> Root;
    };

    class ASTParser : public BaseParserWithFile
    {
        DECLARE_CLASS(ASTParser)
    public:
        ASTParser(const Re::SharedPtr<ASTNodeParser>& lexer)
            : Lexer(lexer)
        {
        }

        bool CompileDeclaration(ICodeFile* file, const Token& token) override;
    private:
        ASTTree Tree;
        Re::SharedPtr<ASTNodeParser> Lexer;
    };
}
