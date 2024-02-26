#pragma once

#include "ReCodeParserDefine.h"
#include "ReClassInfo.h"
#include "Private/Internal/BaseParser.h"

namespace ReParser::Ini
{
	template<typename T>
	struct Single
	{
		T Value{};
	};

	class InitParser;

	class IniFile;
	using IniFilePtr = Re::SharedPtr<IniFile>;
	class IniSectionItem;
	using IniSectionItemPtr = Re::SharedPtr<IniSectionItem>;
	class IniSection;
	using IniSectionPtr = Re::SharedPtr<IniSection>;

	enum class IniSectionItemType
	{
		String,
		Single,
		List,
		Map
	};

	class RECODEPARSER_API IniSectionItem
	{
		DECLARE_CLASS(IniSectionItem)
	public:
		virtual ~IniSectionItem() = default;
		virtual IniSectionItemType GetType() const = 0;

		static IniSectionItemPtr CreateString(const Re::String& content);
		static IniSectionItemPtr CreateSingle();
		static IniSectionItemPtr CreateList();
		static IniSectionItemPtr CreateMap();


		virtual Re::String* GetString() { return nullptr; }
		virtual Single<IniSectionItemPtr>* GetSingle() { return nullptr; }
		virtual Re::Vector<IniSectionItemPtr>* GetList() { return nullptr; }
		virtual Re::Map<Re::String, IniSectionItemPtr>* GetMap() { return nullptr; }

		virtual Re::String ToString() const { return ""; }

	protected:


	};

	class RECODEPARSER_API IniSectionStringItem : public IniSectionItem
	{
		DECLARE_DERIVED_CLASS(IniSectionStringItem, IniSectionItem)
	public:
		explicit IniSectionStringItem(const Re::String& content)
		{
		 Str = content;
		}
		IniSectionItemType GetType() const override { return IniSectionItemType::String; }

		Re::String* GetString() override;

		Re::String ToString() const override { return RE_FORMAT("%s", Str.c_str()); }

	private:
		Re::String Str{};
	};

	class RECODEPARSER_API IniSectionSingleItem : public IniSectionItem
	{
		DECLARE_DERIVED_CLASS(IniSectionSingleItem, IniSectionItem)
	public:
		IniSectionItemType GetType() const override { return IniSectionItemType::Single; }

		Re::String* GetString() override;
		Single<IniSectionItemPtr>* GetSingle() override;
		Re::Vector<IniSectionItemPtr>* GetList() override;
		Re::Map<Re::String, IniSectionItemPtr>* GetMap() override;

		Re::String ToString() const override;

	private:
		Single<IniSectionItemPtr> Item{};
	};

	class RECODEPARSER_API IniSectionListItem : public IniSectionItem
	{
		DECLARE_DERIVED_CLASS(IniSectionListItem, IniSectionItem)
	public:
		IniSectionListItem()
		{
		}
		IniSectionItemType GetType() const override { return IniSectionItemType::List; }
		Re::Vector<IniSectionItemPtr>* GetList() override;

		Re::String ToString() const override;
	private:
		Re::Vector<IniSectionItemPtr> List;
	};

	class RECODEPARSER_API IniSectionMapItem : public IniSectionItem
	{
		DECLARE_DERIVED_CLASS(IniSectionMapItem, IniSectionItem)
	public:
		IniSectionItemType GetType() const override { return IniSectionItemType::Map; }
		Re::Map<Re::String, IniSectionItemPtr>* GetMap() override;

		Re::String ToString() const override;
	private:
		Re::Map<Re::String, IniSectionItemPtr> Map{};
	};

	class RECODEPARSER_API IniSection
	{
	public:
		explicit IniSection(Re::String name) : Name(std::move(name)) {}

		const Re::String& GetName() const { return Name; }

		bool AddItem(const Re::String& itemName, const IniSectionItemPtr& ptr)
		{
			if(Properties.find(itemName) != Properties.end())
			{
				return false;
			}
			Properties.insert(RE_MAKE_PAIR(itemName, ptr));
			return true;
		}

		IniSectionItem* GetItem(const Re::String& itemName)
		{
			auto it = Properties.find(itemName);
			if (it != Properties.end())
			{
				return Re::SharedPtrGet(it->second);
			}
			return nullptr;
		}
		Re::Map<Re::String, IniSectionItemPtr>& GetProperties()
		{
			return Properties;
		}
	private:
		Re::String Name;
		Re::Map<Re::String, IniSectionItemPtr> Properties;
	};

	class RECODEPARSER_API IniFile : public ICodeFile
	{
		DECLARE_DERIVED_CLASS(IniFile, ICodeFile)
	public:
		explicit IniFile(const Re::String& filePath)
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
	public:

		 static IniFilePtr Parse(const Re::String& filePath);

		 void OnNextToken(BaseParser& parser, const Token& token) override { }

		 IniSection* operator[] (const Re::String& sectionName) const
		 {
			 auto it = Sections.find(sectionName);
			 if(it != Sections.end())
			 {
				 return Re::SharedPtrGet(it->second);
			 }
			 return nullptr;
		 }

		 bool AddSection(const Re::String& name, const IniSectionPtr& Section)
		 {
			 if(Sections.find(name) != Sections.end())
			 {
				 return false;
			 }
			 Sections.insert(RE_MAKE_PAIR(name, Section));
			 return true;
		 }

		const Re::String& GetFilePath() const override { return FilePath; }
		const Re::String& GetContent() const override { return Content; }

		const Re::Map<Re::String, Re::SharedPtr<IniSection>>& GetSections() const
		{
		return Sections;
		}

		Re::String ToString() const;

	private:
		Re::String FilePath;
		Re::String Content;
		Re::Map<Re::String, Re::SharedPtr<IniSection>> Sections;
	};

}
