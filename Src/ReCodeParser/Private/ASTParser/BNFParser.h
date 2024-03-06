#pragma once
#include "Internal/BaseParser.h"

namespace ReParser::AST
{
    class ASTNodeParser;
}

namespace ReParser::BNF
{
/** sample
 *    <postal-address> ::= <name-part> <street-address> <zip-part>
 *
 *         <name-part> ::= <personal-part> <last-name> <opt-suffix-part> <EOL> | <personal-part> <name-part>
 *
 *     <personal-part> ::= <first-name> | <initial> "."
 *
 *    <street-address> ::= <house-num> <street-name> <opt-apt-num> <EOL>
 *
 *          <zip-part> ::= <town-name> "," <state-code> <ZIP-code> <EOL>
 *
 *   <opt-suffix-part> ::= "Sr." | "Jr." | <roman-numeral> | ""
 *       <opt-apt-num> ::= "Apt" <apt-num> | ""
 *
 *          ::=     //翻译成人话就是：“定义为”
 *          <A>     //A为必选项
 *          “A”     //A是一个术语，不用翻译
 *          'A'     //A是一个术语，不用翻译
 *          [A]     //A是可选项
 *          {A}     //A是重复项，可出现任意次数，包括0次
 *          A*      //A是重复项，可出现任意次数，包括0次
 *          A+      //A可出现1次或多次
 *          (A B)   //A和B被组合在一起
 *          A|B     //A、B是并列选项，只能选一个

 **/

    class BNFFile : public ICodeFile
    {
        DECLARE_DERIVED_CLASS(BNFFile, ICodeFile)
    public:
        static Re::SharedPtr<BNFFile> Parse(const Re::String& filePath);
        bool AppendRule(const Re::String& ruleName, AST::ASTNodeParser** outParserPtr);
    private:
        Re::Map<Re::String, Re::SharedPtr<AST::ASTNodeParser>> RuleLexers;
    };

    class BNFParser : public BaseParserWithFile
    {
        enum class ParseState
        {
            Global,
            Left,
            Right
        };

        bool CompileDeclaration(ICodeFile* file, const Token& token) override;
    private:

        bool ParseGlobal(BNFFile& file, const Token& token);
        bool ParseLeft(BNFFile& file, const Token& token);
        bool ParseRight(BNFFile& file, const Token& token);

    private:

        int32 LastLine = 0;
        ParseState CurrentState = ParseState::Global;
        Re::Stack<AST::ASTNodeParser*> ParserStack;
    };
}
