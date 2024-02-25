#pragma once
#include "Internal/BaseParser.h"

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
    
    class BNFParser : public BaseParser
    {
        enum class ParseState
        {
            Global,
            Left,
            Right
        };

        bool CompileDeclaration(const Token& token) override;
    private:
        int32 LastLine = 0;
        ParseState CurrentState = ParseState::Global;
    };
}
