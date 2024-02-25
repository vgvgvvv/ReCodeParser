#include "BNFParser.h"

namespace ReParser::BNF
{


    bool BNFParser::CompileDeclaration(const Token& token)
    {
        auto Result = false;
        if(CurrentState == ParseState::Global)
        {
            if(!token.Matches('<'))
            {
                SetError(RE_FORMAT("BNF line should start with '<'", GetLocation()));
                return false;
            }
            UngetToken(token);
            CurrentState = ParseState::Left;
            return true;
        }
        else if(CurrentState == ParseState::Left)
        {
            if(!token.Matches('<'))
            {
                SetError(RE_FORMAT("BNF line should start with '<'", GetLocation()));
                return false;
            }

        }
        else if(CurrentState == ParseState::Right)
        {

        }
        else
        {
            SetError(RE_FORMAT("Unknown BNF State, failed to parse !! %s", GetLocation().c_str()));
            return false;
        }



    }
}
