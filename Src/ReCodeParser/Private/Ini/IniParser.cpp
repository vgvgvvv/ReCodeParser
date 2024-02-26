#include "IniParser.h"
#include <ReClassMisc.h>

namespace ReParser::Ini
{
    DEFINE_CLASS_WITHOUT_NEW(IniSectionItem)
    DEFINE_DERIVED_CLASS_WITHOUT_NEW(IniSectionStringItem, IniSectionItem)
    DEFINE_DERIVED_CLASS_WITHOUT_NEW(IniSectionSingleItem, IniSectionItem)
    DEFINE_DERIVED_CLASS_WITHOUT_NEW(IniSectionListItem, IniSectionItem)
    DEFINE_DERIVED_CLASS_WITHOUT_NEW(IniSectionMapItem, IniSectionItem)

    DEFINE_DERIVED_CLASS_WITHOUT_NEW(IniFile, ICodeFile)

    IniSectionItemPtr IniSectionItem::CreateString(const Re::String& content)
    {
        return Re::MakeShared<IniSectionStringItem>(content);
    }

    IniSectionItemPtr IniSectionItem::CreateSingle()
    {
        return Re::MakeShared<IniSectionSingleItem>();
    }

    IniSectionItemPtr IniSectionItem::CreateList()
    {
        return Re::MakeShared<IniSectionListItem>();
    }

    IniSectionItemPtr IniSectionItem::CreateMap()
    {
        return Re::MakeShared<IniSectionMapItem>();
    }

    Re::String* IniSectionStringItem::GetString()
    {
        return &Str;
    }

    Re::String* IniSectionSingleItem::GetString()
    {
        if(auto single = GetSingle())
        {
            if(auto ptr = Re::SharedPtrGet(single->Value))
            {
                return ptr->GetString();
            }
        }
        return nullptr;
    }

    Single<IniSectionItemPtr>* IniSectionSingleItem::GetSingle()
    {
        return &Item;
    }

    Re::Vector<IniSectionItemPtr>* IniSectionSingleItem::GetList()
    {
        if (auto single = GetSingle())
        {
            if (auto ptr = Re::SharedPtrGet(single->Value))
            {
                return ptr->GetList();
            }
        }
        return nullptr;
    }

    Re::Map<Re::String, IniSectionItemPtr>* IniSectionSingleItem::GetMap()
    {
        if (auto single = GetSingle())
        {
            if (auto ptr = Re::SharedPtrGet(single->Value))
            {
                return ptr->GetMap();
            }
        }
        return nullptr;
    }

    Re::String IniSectionSingleItem::ToString() const
    {
        if(!Item.Value)
        {
            return "(null)";
        }
        return Item.Value->ToString();
    }

    Re::Vector<IniSectionItemPtr>* IniSectionListItem::GetList()
    {
        return &List;
    }

    Re::String IniSectionListItem::ToString() const
    {
        Re::String ListBuilder = "[";
        bool isFirst = true;
        for (auto& item : List)
        {
            if(!isFirst)
            {
                ListBuilder += ", ";
            }
            ListBuilder += item->ToString();
            isFirst = false;
        }
        ListBuilder += "]";
        return ListBuilder;
    }

    Re::Map<Re::String, IniSectionItemPtr>* IniSectionMapItem::GetMap()
    {
        return &Map;
    }

    Re::String IniSectionMapItem::ToString() const
    {
        Re::String MapBuilder = "(";
        bool isFirst = true;
        for (auto& pair : Map)
        {
            if(!isFirst)
            {
                MapBuilder += ", ";
            }
            MapBuilder += pair.first;
            MapBuilder += "=";
            MapBuilder += pair.second->ToString();
            isFirst = false;
        }
        MapBuilder += ")";
        return MapBuilder;
    }

    class IniParser : public BaseParserWithFile
    {
        enum class IniScopeType
        {
            File,
            Section,
            SectionItem
        };
        struct IIniScope
        {
            virtual ~IIniScope() = default;
            virtual IniScopeType GetType() const = 0;
        };
        struct IniFileScope : public IIniScope
        {
            IniScopeType GetType() const override { return IniScopeType::File; }
            explicit IniFileScope(IniFile* file)
            {
                File = file;
            }
            IniFile* File = nullptr;
        };
        struct IniSectionScope : public IIniScope
        {
            IniScopeType GetType() const override { return IniScopeType::Section; }
            explicit IniSectionScope(IniSection* section)
            {
                Section = section;
            }
            IniSection* Section = nullptr;
        };
        struct IniSectionItemScope : public IIniScope
        {
            IniScopeType GetType() const override { return IniScopeType::SectionItem; }
            explicit IniSectionItemScope(IniSectionItem* item)
            {
                Item = item;
            }
            IniSectionItem* Item = nullptr;
        };
    public:
        void PreParserProcess(ICodeFile* file) override;
        void PostParserProcess(ICodeFile* file) override;
        bool CompileDeclaration(ICodeFile* file, const Token& token) override;
    protected:
        bool IsBeginComment(char currentChar) override { return false; }
        bool IsEndComment(char currentChar) override { return false; }
        bool IsLineComment(char currentChar) override { return currentChar == '#' || currentChar == ';'; }
    private:
        bool CompileFileScope(ICodeFile& file, IniFileScope& fileScope, const Token& token);
        bool CompileSectionScope(ICodeFile& file, IniSectionScope& sectionScope, const Token& token);
        bool CompileSectionItemScope(ICodeFile& file, IniSectionItemScope& sectionItemScope, const Token& token);

        IniSectionItemPtr ParseValue(ICodeFile& file, const Token& token);
        bool ParseMap(ICodeFile& file);
        bool ParseList(ICodeFile& file);

    private:
        Re::Stack<Re::SharedPtr<IIniScope>> ScopeStack;
    };


    IniFilePtr IniFile::Parse(const Re::String& filePath)
    {
        auto result = Re::MakeShared<IniFile>(filePath);
        IniParser parser;
        parser.InitParserSource(result->GetFilePath(), result->GetContent().c_str());
        parser.Parse(Re::SharedPtrGet(result));
        return result;
    }

    Re::String IniFile::ToString() const
    {
        Re::String result;

        for (auto& sectionPair : Sections)
        {
            auto& name = sectionPair.first;
            auto& section = sectionPair.second;
            result += RE_FORMAT("[%s]\n", name.c_str());
            for (const auto & property : section->GetProperties())
            {
                Re::String PropertyBuilder;
                PropertyBuilder += property.first;
                PropertyBuilder += " -> ";
                PropertyBuilder += property.second->ToString();
                result += RE_FORMAT("\t%s\n", PropertyBuilder.c_str());
            }

            result += "\n";
        }

        return result;
    }

    void IniParser::PreParserProcess(ICodeFile* file)
    {
        if(file == nullptr)
        {
            SetError("code file is null !!");
            return;
        }

        auto iniFile = ReClassSystem::CastTo<IniFile>(file);
        if(!iniFile)
        {
            SetError(RE_FORMAT("current code file is not a ini file !! %s", file->GetFilePath().c_str()));
            return;
        }
        ScopeStack.push(RE_MOVE(Re::MakeShared<IniFileScope>(iniFile)));
    }

    void IniParser::PostParserProcess(ICodeFile* file)
    {
        if(!file)
        {
            return;
        }
        Re::String err;
        if(GetError(err))
        {
            RE_LOG_F("parse %s failed !! reason: %s", file->GetFilePath().c_str(), err.c_str())
            return;
        }
        RE_ASSERT(ScopeStack.size() == 2 && ScopeStack.top() && ScopeStack.top()->GetType() == IniScopeType::Section);
        ScopeStack.pop();

        RE_ASSERT(ScopeStack.size() == 1 && ScopeStack.top() && ScopeStack.top()->GetType() == IniScopeType::File);
        ScopeStack.pop();
    }

    bool IniParser::CompileDeclaration(ICodeFile* file, const Token& token)
    {
        auto& currentScope = ScopeStack.top();
        if(currentScope && file)
        {
            switch (currentScope->GetType())
            {
            case IniScopeType::File:
                {
                    auto scope = Re::SharedPtrCast<IniFileScope>(currentScope);
                    return CompileFileScope(*file, *scope, token);
                }
            case IniScopeType::Section:
                {
                    auto scope = Re::SharedPtrCast<IniSectionScope>(currentScope);
                    return CompileSectionScope(*file, *scope, token);
                }
            case IniScopeType::SectionItem:
                {
                    auto scope = Re::SharedPtrCast<IniSectionItemScope>(currentScope);
                    return CompileSectionItemScope(*file, *scope, token);
                }
            default:
                {
                    SetError(RE_FORMAT("unexcepted scope !!! %s", GetFileLocation(file).c_str()));
                    return false;
                }
            }
        }
        else
        {
            SetError(RE_FORMAT("scope or file is null !!! %s", GetFileLocation(file).c_str()));
            return false;
        }
    }

    bool IniParser::CompileFileScope(ICodeFile& file, IniFileScope& fileScope, const Token& token)
    {
        RE_ASSERT(token.Matches('['));
        Re::String nameBuilder;
        while(true)
        {
            auto nameToken = GetToken(true);
            if(!nameToken)
            {
                SetError(RE_FORMAT("unexcepted end of file %s", GetFileLocation(&file)));
                return false;
            }
            if(nameToken->Matches(']'))
            {
                if(nameBuilder.empty())
                {
                    SetError(RE_FORMAT("ini section item name is empty %s", GetFileLocation(&file)));
                    return false;
                }
                break;
            }
            nameBuilder += nameToken->GetTokenName();
        }

        auto newSection = Re::MakeShared<IniSection>(nameBuilder);
        if(!fileScope.File->AddSection(nameBuilder, newSection))
        {
            SetError(RE_FORMAT("ini section %s has added !!", nameBuilder.c_str()));
            return false;
        }

        ScopeStack.push(Re::MakeShared<IniSectionScope>(Re::SharedPtrGet(newSection)));

        return true;
    }

    bool IniParser::CompileSectionScope(ICodeFile& file, IniSectionScope& sectionScope, const Token& token)
    {
        // meet next section
        if(token.Matches('['))
        {
            ScopeStack.pop();
            UngetToken(token);
            return true;
        }

        bool isList = token.Matches('+');
        Re::String sectionNameBuilder;

        Token currentNameToken;
        if(isList)
        {
            auto nextNameToken = GetToken(true);
            const Token* sectionItemNameTokenPtr = Re::SharedPtrGet(nextNameToken);
            if(!sectionItemNameTokenPtr)
            {
                SetError(RE_FORMAT("unexpected end of file %s", GetFileLocation(&file).c_str()));
                return false;
            }
            currentNameToken = *sectionItemNameTokenPtr;
        }
        else
        {
            currentNameToken = token;
        }

        while(!currentNameToken.Matches('='))
        {
            if(currentNameToken.GetTokenType() == ETokenType::Const)
            {
                SetError(RE_FORMAT("section item name cannot be const value !! %s", GetFileLocation(&file).c_str()));
                return false;
            }
            sectionNameBuilder += currentNameToken.GetTokenName();
            auto nextToken = GetToken(true);
            if(!nextToken)
            {
                SetError(RE_FORMAT("unexpected end of file %s", GetFileLocation(&file).c_str()));
                return false;
            }
            currentNameToken = *nextToken;
        }

        IniSectionItem* item = nullptr;
        if(isList)
        {
            item = sectionScope.Section->GetItem(sectionNameBuilder);
            if(!item)
            {
                auto newItem = IniSectionItem::CreateList();
                item = Re::SharedPtrGet(newItem);
                sectionScope.Section->AddItem(sectionNameBuilder, RE_MOVE(newItem));
            }
        }
        else
        {
            item = sectionScope.Section->GetItem(sectionNameBuilder);
            if(item != nullptr)
            {
                SetError(RE_FORMAT("section item %s has been added %s", sectionNameBuilder.c_str(), GetFileLocation(&file).c_str()));
                return false;
            }
            auto newItem = IniSectionItem::CreateSingle();
            item = Re::SharedPtrGet(newItem);
            sectionScope.Section->AddItem(sectionNameBuilder, RE_MOVE(newItem));
        }

        ScopeStack.push(Re::MakeShared<IniSectionItemScope>(item));

        return true;
    }

    bool IniParser::CompileSectionItemScope(ICodeFile& file, IniSectionItemScope& sectionItemScope, const Token& token)
    {
        auto& item = sectionItemScope.Item;
        RE_ASSERT(item);
        // string type should never reach here
        RE_ASSERT(item->GetType() != IniSectionItemType::String);

        if(item->GetType() == IniSectionItemType::Single)
        {
            RE_ASSERT(item->GetSingle());
            item->GetSingle()->Value = ParseValue(file, token);
            ScopeStack.pop();
            return true;
        }
        else if(item->GetType() == IniSectionItemType::List)
        {
            RE_ASSERT(item->GetList());
            auto result = ParseValue(file, token);
            if (!result)
            {
                SetError(RE_FORMAT("parse ini section item failed !! %s", GetFileLocation(&file).c_str()));
            }
            item->GetList()->emplace_back(RE_MOVE(result));
            ScopeStack.pop();
            return true;
        }
        else if(item->GetType() == IniSectionItemType::Map)
        {
            // can never reach here now, but remain here for future features?
            if(token.GetTokenType() != ETokenType::Identifier)
            {
                SetError(RE_FORMAT("must start with a identify !! %s", GetFileLocation(&file).c_str()));
                return false;
            }

            auto name = token.GetTokenName();
            RequireSymbol('=', "ini parse");
            auto result = ParseValue(file, token);
            if(!result)
            {
                SetError(RE_FORMAT("parse ini section item failed !! %s", GetFileLocation(&file).c_str()));
            }
            item->GetMap()->emplace(name, RE_MOVE(result));
            ScopeStack.pop();

            return true;
        }
        else
        {
            // should never reach
            RE_ASSERT(false);
            SetError("unknown error !! please review parser code..");
            return false;
        }
    }

    IniSectionItemPtr IniParser::ParseValue(ICodeFile& file, const Token& token)
    {
        auto currentLine = InputLine;
        IniSectionItemPtr newItem;
        if(token.Matches('('))
        {
            UngetToken(token);
            newItem = IniSectionItem::CreateMap();
            ScopeStack.push(Re::MakeShared<IniSectionItemScope>(Re::SharedPtrGet(newItem)));
            if(!ParseMap(file))
            {
                return nullptr;
            }
        }
        else if(token.Matches('['))
        {
            UngetToken(token);
            newItem = IniSectionItem::CreateList();
            ScopeStack.push(Re::MakeShared<IniSectionItemScope>(Re::SharedPtrGet(newItem)));
            if(!ParseList(file))
            {
                return nullptr;
            }
        }
        else
        {
            auto itemContentToken = token;
            Re::String itemContentBuilder;
            if(token.GetTokenType() == ETokenType::Identifier || token.GetTokenType() == ETokenType::Symbol)
            {
                itemContentBuilder += itemContentToken.GetTokenName();
                while(!itemContentToken.Matches(')') && !itemContentToken.Matches(']') && !itemContentToken.Matches(','))
                {
                    if(itemContentToken.GetTokenType() != ETokenType::Identifier)
                    {
                        SetError(RE_FORMAT("invalid token type !! %s", GetFileLocation(&file).c_str()));
                        return nullptr;
                    }
                    auto nextToken = GetToken(true);
                    if(!nextToken)
                    {
                       break;
                    }
                    if(InputLine != currentLine)
                    {
                        UngetToken(nextToken);
                        break;
                    }
                    itemContentToken = *nextToken;
                    itemContentBuilder += itemContentToken.GetTokenName();
                }
            }
            else if(token.GetTokenType() == ETokenType::Const)
            {
                itemContentBuilder += itemContentToken.GetConstantValue();
            }
            else
            {
                SetError(RE_FORMAT("unexpected token type %s", GetFileLocation(&file).c_str()));
                return nullptr;
            }
            newItem = IniSectionItem::CreateString(itemContentBuilder);
        }

        return newItem;
    }

    bool IniParser::ParseMap(ICodeFile& file)
    {
        MatchSymbol('(');
        RE_ASSERT(!ScopeStack.empty() && ScopeStack.top());
        auto scopeItem = ScopeStack.top();
        RE_ASSERT(scopeItem->GetType() == IniScopeType::SectionItem);
        auto mapItem = Re::SharedPtrCast<IniSectionItemScope>(scopeItem);

        while(true)
        {
            auto token = GetToken();
            if(!token)
            {
                SetError(RE_FORMAT("unexpected end of file %s", GetFileLocation(&file).c_str()));
                return false;
            }

            if(token->Matches(')'))
            {
                ScopeStack.pop();
                break;
            }

            if(token->Matches(','))
            {
                continue;
            }

            auto mapItemNameToken = token;
            Re::String mapItemNameBuilder;
            while(!mapItemNameToken->Matches('='))
            {
                if(mapItemNameToken->GetTokenType() == ETokenType::Const)
                {
                    SetError(RE_FORMAT("invalid map item name %s : %s", mapItemNameToken->GetConstantValue().c_str(), GetFileLocation(&file).c_str()));
                    return false;
                }
                mapItemNameBuilder += mapItemNameToken->GetTokenName();
                mapItemNameToken = GetToken(true);

                if(!mapItemNameToken)
                {
                    SetError(RE_FORMAT("unexpected end of file %s", GetFileLocation(&file).c_str()));
                    return false;
                }
            }

            auto nextToken = GetToken();
            if (!nextToken)
            {
                SetError(RE_FORMAT("unexpected end of file %s", GetFileLocation(&file).c_str()));
                return false;
            }

            auto subItem = ParseValue(file, *nextToken);
            if(!subItem)
            {
                SetError(RE_FORMAT("failed to parse section item %s", GetFileLocation(&file).c_str()));
                return false;
            }

            auto map = mapItem->Item->GetMap();
            RE_ASSERT(map);
            if(map->find(mapItemNameBuilder) != map->end())
            {
                SetError(RE_FORMAT("repeat key in map item !! %s", GetFileLocation(&file).c_str()));
                return false;
            }
            map->emplace(mapItemNameBuilder, RE_MOVE(subItem));
        }

        return true;
    }

    bool IniParser::ParseList(ICodeFile& file)
    {
        MatchSymbol('[');
        RE_ASSERT(!ScopeStack.empty() && ScopeStack.top());
        auto scopeItem = ScopeStack.top();
        RE_ASSERT(scopeItem->GetType() == IniScopeType::SectionItem);
        auto listItem = Re::SharedPtrCast<IniSectionItemScope>(scopeItem);

        while(true)
        {
            auto token = GetToken();

            if(!token)
            {
                SetError(RE_FORMAT("unexpected end of file %s", GetFileLocation(&file).c_str()));
                return false;
            }

            if(token->Matches(']'))
            {
                ScopeStack.pop();
                break;
            }

            if(token->Matches(','))
            {
                continue;
            }

            auto subItem = ParseValue(file, *token);
            if (!subItem)
            {
                SetError(RE_FORMAT("failed to parse section item %s", GetFileLocation(&file).c_str()));
                return false;
            }

            auto list = listItem->Item->GetList();
            RE_ASSERT(list);
            list->emplace_back(RE_MOVE(subItem));

        }


        return true;
    }
}

