#pragma once
#include "ASTParser.h"

namespace ReParser::AST
{
// parser below is used for BNF

    // 'A' or "A"
    class RequiredIdentifierNodeParser : public ASTNodeParser
    {
        DECLARE_DERIVED_CLASS(RequiredIdentifierNodeParser, ASTNodeParser)
    public:
        bool Parse(ICodeFile* file, ASTParser& context, const Token& token, ASTNodePtr* outNode) override;
        Re::String ToString() const override;
    private:
        Re::String TokenName;
    };

    // A | B
    class OrNodeParser : public ASTNodeParser
    {
        DECLARE_DERIVED_CLASS(OrNodeParser, ASTNodeParser)
    public:
        bool Parse(ICodeFile* file, ASTParser& context, const Token& token, ASTNodePtr* outNode) override;
        void AddRule(const Re::SharedPtr<ASTNodeParser>& rule) { SubRules.push_back(rule); }
        Re::String ToString() const override;
    private:
        Re::Vector<Re::SharedPtr<ASTNodeParser>> SubRules;
    };

    // (A B)
    class GroupNodeParser : public ASTNodeParser
    {
        DECLARE_DERIVED_CLASS(GroupNodeParser, ASTNodeParser)
    public:
        bool Parse(ICodeFile* file, ASTParser& context, const Token& token, ASTNodePtr* outNode) override;
        void AddRule(const Re::SharedPtr<ASTNodeParser>& rule) { SubRules.push_back(rule); }
        const Re::Vector<Re::SharedPtr<ASTNodeParser>>& GetSubRules() { return SubRules; }
        Re::String ToString() const override;
    private:
        Re::Vector<Re::SharedPtr<ASTNodeParser>> SubRules;
    };

    // [A] is optional
    class OptionNodeParser : public ASTNodeParser
    {
        DECLARE_DERIVED_CLASS(OptionNodeParser, ASTNodeParser)
    public:
        bool Parse(ICodeFile* file, ASTParser& context, const Token& token, ASTNodePtr* outNode) override;
        Re::String ToString() const override;
    private:
        Re::SharedPtr<ASTNodeParser> SubRule;
    };

    // {A} or A*
    class OptionalRepeatNodeParser : public ASTNodeParser
    {
        DECLARE_DERIVED_CLASS(OptionalRepeatNodeParser, ASTNodeParser)
    public:
        bool Parse(ICodeFile* file, ASTParser& context, const Token& token, ASTNodePtr* outNode) override;
        Re::String ToString() const override;
    private:
        Re::SharedPtr<ASTNodeParser> SubRule;
    };

    // A+
    class RepeatNodeParser : public ASTNodeParser
    {
        DECLARE_DERIVED_CLASS(RepeatNodeParser, ASTNodeParser)
    public:
        bool Parse(ICodeFile* file, ASTParser& context, const Token& token, ASTNodePtr* outNode) override;
        Re::String ToString() const override;
    private:
        Re::SharedPtr<ASTNodeParser> SubRule;
    };

}