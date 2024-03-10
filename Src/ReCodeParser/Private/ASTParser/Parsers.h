#pragma once
#include "ASTParser.h"

namespace ReParser::AST
{
// parser below is used for BNF

    // 'A' or "A"
    class RECODEPARSER_API RequiredIdentifierNodeParser : public ASTNodeParser
    {
        DECLARE_DERIVED_CLASS(RequiredIdentifierNodeParser, ASTNodeParser)
    public:
        explicit RequiredIdentifierNodeParser(const Re::String& name)
          : TokenName(name)
        {
        }
        bool Parse(ICodeFile* file, ASTParser& context, const Token& token, ASTNodePtr* outNode) override;
        Re::String ToString() const override;
    private:
        Re::String TokenName{};
    };

    // A | B
    class RECODEPARSER_API OrNodeParser : public ASTNodeParser
    {
        DECLARE_DERIVED_CLASS(OrNodeParser, ASTNodeParser)
    public:
        bool Parse(ICodeFile* file, ASTParser& context, const Token& token, ASTNodePtr* outNode) override;
        void AddRule(const Re::SharedPtr<ASTNodeParser>& rule) { SubRules.push_back(rule); }
        void ClearRules() { SubRules.clear(); }
        Re::String ToString() const override;
    private:
        Re::Vector<Re::SharedPtr<ASTNodeParser>> SubRules;
    };

    // (A B)
    class RECODEPARSER_API GroupNodeParser : public ASTNodeParser
    {
        DECLARE_DERIVED_CLASS(GroupNodeParser, ASTNodeParser)
    public:
        bool Parse(ICodeFile* file, ASTParser& context, const Token& token, ASTNodePtr* outNode) override;
        void AddRule(const Re::SharedPtr<ASTNodeParser>& rule) { SubRules.push_back(rule); }
        const Re::Vector<Re::SharedPtr<ASTNodeParser>>& GetSubRules() { return SubRules; }
        void ClearRules() { SubRules.clear(); }
        Re::String ToString() const override;
    private:
        Re::Vector<Re::SharedPtr<ASTNodeParser>> SubRules;
    };

    class RECODEPARSER_API CustomNodeParser : public ASTNodeParser
    {
        DECLARE_DERIVED_CLASS(CustomNodeParser, ASTNodeParser)
    public:
        explicit CustomNodeParser(const Re::String& customParserName)
            : CustomParserName(customParserName)
        {
        }
        bool Parse(ICodeFile* file, ASTParser& context, const Token& token, ASTNodePtr* outNode) override;
    private:
        Re::SharedPtr<ASTNodeParser> RealParser{};
        Re::String CustomParserName{};
    };

    // [A] is optional
    class RECODEPARSER_API OptionNodeParser : public ASTNodeParser
    {
        DECLARE_DERIVED_CLASS(OptionNodeParser, ASTNodeParser)
    public:
        explicit OptionNodeParser(const Re::SharedPtr<ASTNodeParser>& subRule)
            : SubRule(subRule)
        {
        }
        bool Parse(ICodeFile* file, ASTParser& context, const Token& token, ASTNodePtr* outNode) override;
        Re::String ToString() const override;
    private:
        Re::SharedPtr<ASTNodeParser> SubRule{};
    };

    // {A} or A* optional repeat
    class RECODEPARSER_API OptionalRepeatNodeParser : public ASTNodeParser
    {
        DECLARE_DERIVED_CLASS(OptionalRepeatNodeParser, ASTNodeParser)
    public:
        explicit OptionalRepeatNodeParser(const Re::SharedPtr<ASTNodeParser>& subRule)
            : SubRule(subRule)
        {
        }
        bool Parse(ICodeFile* file, ASTParser& context, const Token& token, ASTNodePtr* outNode) override;
        Re::String ToString() const override;
    private:
        Re::SharedPtr<ASTNodeParser> SubRule;
    };

    // A+ at least one repeat
    class RECODEPARSER_API RepeatNodeParser : public ASTNodeParser
    {
        DECLARE_DERIVED_CLASS(RepeatNodeParser, ASTNodeParser)
    public:
        explicit RepeatNodeParser(const Re::SharedPtr<ASTNodeParser>& subRule)
          : SubRule(subRule)
        {
        }
        bool Parse(ICodeFile* file, ASTParser& context, const Token& token, ASTNodePtr* outNode) override;
        Re::String ToString() const override;
    private:
        Re::SharedPtr<ASTNodeParser> SubRule;
    };

}