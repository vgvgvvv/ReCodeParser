#pragma once
#include "ReClassInfo.h"
#include "Private/Internal/BaseParser.h"

namespace ReParser::AST
{
    class ASTParser;
    class ASTNode;

    using ASTNodePtr = Re::SharedPtr<ASTNode>;
    class RECODEPARSER_API ASTNode
    {
        DECLARE_CLASS(ASTNode)
    public:
        virtual ~ASTNode() = default;
        virtual void GetChildNodes(Re::Vector<ASTNodePtr>& outChildren) const = 0;
        Re::List<ASTNode*> GetChildNodesWithList() const
        {
            Re::List<ASTNode*> Result;
            Re::Vector<ASTNodePtr> Children;
            GetChildNodes(Children);
            for (const auto & node : Children)
            {
                Result.push_back(Re::SharedPtrGet(node));
            }
            return Result;
        }
        virtual Re::String ToString() const = 0;
        Re::String ToStringNonConst() { return ToString(); }
        Re::List<ASTNode*> GetChildNodesWithListNonConst() { return GetChildNodesWithList(); }
    private:
        Re::String Name;
    };

    template<typename T, class ... Ts>
    Re::SharedPtr<T> CreateASTNode(Ts&& ... args)
    {
        return Re::MakeShared<T>(std::forward<Ts>(args)...);
    }

    class RECODEPARSER_API ASTNodeParser
    {
        DECLARE_CLASS(ASTNodeParser)
    public:
        virtual ~ASTNodeParser() = default;
        virtual bool Parse(ICodeFile* file, ASTParser& context, const Token& token, ASTNodePtr* outNode) = 0;
        virtual Re::String ToString() const { return RE_FORMAT("*%s*", StaticClass().GetName()); }

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

    class RECODEPARSER_API ASTTree
    {
    public:
        bool Parse(ASTNodeParser& parser, ICodeFile* file, ASTParser& context, const Token& token);

        const ASTNode& GetRoot() const { return *Root; }
        Re::String ToString() const;

    private:
        Re::SharedPtr<ASTNode> Root;
    };

    class RECODEPARSER_API ASTParser : public BaseParserWithFile
    {
        DECLARE_CLASS(ASTParser)
    public:
        explicit ASTParser(const Re::SharedPtr<ASTNodeParser>& lexer)
            : Lexer(lexer)
        {
        }

        bool CompileDeclaration(ICodeFile* file, const Token& token) override;
        void AddCustomParser(const Re::String& name, const Re::SharedPtr<ASTNodeParser>& parser);
        bool TryGetCustomParser(const Re::String& name, Re::SharedPtr<ASTNodeParser>* outParser);

        const ASTTree& GetASTTree() const { return Tree; }

    private:
        ASTTree Tree;
        Re::SharedPtr<ASTNodeParser> Lexer;
        Re::Map<Re::String, Re::SharedPtr<ASTNodeParser>> CustomParsers;
    };
}
