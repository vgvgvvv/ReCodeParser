#pragma once
#include "ASTParser.h"

namespace ReParser::AST
{

    // node below is for basic tokens

    // identifier token
    class IdentifierNode : public ASTNode
    {
        DECLARE_DERIVED_CLASS(IdentifierNode, ASTNode)
    public:
        explicit IdentifierNode(const Token& token)
            : IdToken(token)
        {
            RE_ASSERT(token.GetTokenType() == ETokenType::Identifier);
        }

        const Token& GetToken() const
        {
            return IdToken;
        }

    private:
        Token IdToken{};
    };

    class GroupNode : public ASTNode
    {
        DECLARE_DERIVED_CLASS(GroupNode, ASTNode)
    public:
        GroupNode()
        {
        }

        void AppendNode(const ASTNodePtr& node)
        {
            SubNodes.push_back(node);
        }

        const Re::Vector<ASTNodePtr>& GetSubNodes() const
        {
            return SubNodes;
        }

    private:
        Re::Vector<ASTNodePtr> SubNodes;
    };


    // symbol token
    class SymbolNode : public ASTNode
    {
        DECLARE_DERIVED_CLASS(SymbolNode, ASTNode)
    public:
        explicit SymbolNode(const Token& token)
                    : SymbolToken(token)
        {
            RE_ASSERT(token.GetTokenType() == ETokenType::Symbol);
        }
        
        const Token& GetToken() const
        {
            return SymbolToken;
        }

    private:
        Token SymbolToken{};
    };

    // const value
    class ConstNode : public ASTNode
    {
        DECLARE_DERIVED_CLASS(ConstNode, ASTNode)
    public:
        explicit ConstNode(const Token& token)
                           : ConstToken(token)
        {
            RE_ASSERT(token.GetTokenType() == ETokenType::Symbol);
        }

        const Token& GetToken() const
        {
            return ConstToken;
        }

    private:
        Token ConstToken{};
    };

    // number token
    class NumNode : public ConstNode
    {
        DECLARE_DERIVED_CLASS(NumNode, ConstNode)
    public:
        explicit NumNode(const Token& token)
            : SuperClass(token)
        {
            RE_ASSERT(token.GetConstType() == ETokenConstType::Float ||
                token.GetConstType() == ETokenConstType::Double  ||
                token.GetConstType() == ETokenConstType::Int ||
                token.GetConstType() == ETokenConstType::Int64 ||
                token.GetConstType() == ETokenConstType::Byte);
        }
    };

    // string token
    class StringNode : public ConstNode
    {
        DECLARE_DERIVED_CLASS(StringNode, ConstNode)
    public:
        explicit StringNode(const Token& token)
                   : SuperClass(token)
        {
            RE_ASSERT(token.GetConstType() == ETokenConstType::String);
        }
    };

}