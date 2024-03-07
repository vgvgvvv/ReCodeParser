#pragma once
#include "ReCodeParserDefine.h"
#include "Private/Internal/BaseParser.h"

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

    class RECODEPARSER_API BNFFile : public ICodeFile
    {
        DECLARE_DERIVED_CLASS(BNFFile, ICodeFile)
    public:

        explicit BNFFile(const Re::String& filePath)
            : FilePath(filePath)
        {
            if(!std::filesystem::exists(filePath))
            {
                RE_ERROR_F("read ini file %s failed !!", filePath.c_str());
                return;
            }
            std::ifstream t(filePath);
            std::stringstream buffer;
            buffer << t.rdbuf();
            Content = buffer.str();
        }

        explicit BNFFile(const Re::String& filePath, const Re::String& content)
            : FilePath(filePath)
            , Content(content)
        {
        }

        static Re::SharedPtr<BNFFile> Parse(const Re::String& filePath);
        static Re::SharedPtr<BNFFile> Parse(const Re::String& filePath, const Re::String& content);

        using RuleLexersMap = Re::Map<Re::String, Re::SharedPtr<AST::ASTNodeParser>>;
        const RuleLexersMap& GetRuleLexers() const { return RuleLexers; }
        bool AppendRule(const Re::String& ruleName, Re::SharedPtr<AST::ASTNodeParser>* outParserPtr);

        const Re::String& GetFilePath() const override { return FilePath; }
        const Re::String& GetContent() const override { return Content; }

        Re::String ToString() const;

    private:
        Re::String FilePath;
        Re::String Content;
        RuleLexersMap RuleLexers;
    };

}
