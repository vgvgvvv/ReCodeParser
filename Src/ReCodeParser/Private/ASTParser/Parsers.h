#pragma once
#include "ASTParser.h"

namespace ReParser::AST
{
// parser below is used for BNF

    // 'A' or "A"
    class RequiredIdentifierNode : public ASTNodeParser
    {
        DECLARE_DERIVED_CLASS(RequiredIdentifierNode, ASTNodeParser)
    public:
        ASTNodePtr Parse(ICodeFile* file, ASTParser& context, const Token& token) override;

    private:
        Re::String TokenName;
    };

    // A | B
    class OrNode : public ASTNodeParser
    {
        DECLARE_DERIVED_CLASS(OrNode, ASTNodeParser)
    public:
        ASTNodePtr Parse(ICodeFile* file, ASTParser& context, const Token& token) override;
        void AddRule(const Re::WeakPtr<ASTNodeParser>& rule) { SubRules.push_back(rule); }
    private:
        Re::Vector<Re::WeakPtr<ASTNodeParser>> SubRules;
    };

    // (A B)
    class GroupNode : public ASTNodeParser
    {
        DECLARE_DERIVED_CLASS(GroupNode, ASTNodeParser)
    public:
        ASTNodePtr Parse(ICodeFile* file, ASTParser& context, const Token& token) override;
        void AddRule(const Re::WeakPtr<ASTNodeParser>& rule) { SubRules.push_back(rule); }
    private:
        Re::Vector<Re::WeakPtr<ASTNodeParser>> SubRules;
    };

    // [A] is optional
    class OptionNode : public ASTNodeParser
    {
        DECLARE_DERIVED_CLASS(OptionNode, ASTNodeParser)
    public:
        ASTNodePtr Parse(ICodeFile* file, ASTParser& context, const Token& token) override;
    private:
        Re::SharedPtr<ASTNodeParser> RealNode;
    };

    // {A} or A*
    class OptionalRepeatNode : public ASTNodeParser
    {
        DECLARE_DERIVED_CLASS(OptionalRepeatNode, ASTNodeParser)
    public:
        ASTNodePtr Parse(ICodeFile* file, ASTParser& context, const Token& token) override;
    private:
        Re::SharedPtr<ASTNodeParser> RealNode;
    };

    // A+
    class RepeatNode : public ASTNodeParser
    {
        DECLARE_DERIVED_CLASS(RepeatNode, ASTNodeParser)
    public:
        ASTNodePtr Parse(ICodeFile* file, ASTParser& context, const Token& token) override;
    private:
        Re::SharedPtr<ASTNodeParser> RealNode;
    };

}