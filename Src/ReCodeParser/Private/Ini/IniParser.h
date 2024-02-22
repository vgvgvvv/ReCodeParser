#pragma once


#include "ReClassInfo.h"
#include "Private/Internal/BaseParser.h"

namespace ReParser
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
	using IniSectionItemPtr = Re::UniquePtr<IniSectionItem>;
	class IniSection;
	using IniSectionPtr = Re::UniquePtr<IniSection>;

	enum class IniSectionItemType
	{
		String,
		Single,
		List,
		Map
	};

	class IniSectionItem
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

	protected:


	};

	class IniSectionStringItem : public IniSectionItem
	{
		DECLARE_DERIVED_CLASS(IniSectionStringItem, IniSectionItem)
	public:
		explicit IniSectionStringItem(const Re::String& content)
		{
		 Str = content;
		}
		IniSectionItemType GetType() const override { return IniSectionItemType::String; }

		Re::String* GetString() override;

	private:
		Re::String Str{};
	};

	class IniSectionSingleItem : public IniSectionItem
	{
		DECLARE_DERIVED_CLASS(IniSectionSingleItem, IniSectionItem)
	public:
		IniSectionItemType GetType() const override { return IniSectionItemType::Single; }

		Re::String* GetString() override;
		Single<IniSectionItemPtr>* GetSingle() override;
		Re::Vector<IniSectionItemPtr>* GetList() override;
		Re::Map<Re::String, IniSectionItemPtr>* GetMap() override;
	private:
		Single<IniSectionItemPtr> Item{};
	};

	class IniSectionListItem : public IniSectionItem
	{
		DECLARE_DERIVED_CLASS(IniSectionListItem, IniSectionItem)
	public:
		IniSectionListItem()
		{
		}
		IniSectionItemType GetType() const override { return IniSectionItemType::List; }
		Re::Vector<IniSectionItemPtr>* GetList() override;
	private:
		Re::Vector<IniSectionItemPtr> List{};
	};

	class IniSectionMapItem : public IniSectionItem
	{
		DECLARE_DERIVED_CLASS(IniSectionMapItem, IniSectionItem)
	public:
		IniSectionItemType GetType() const override { return IniSectionItemType::Map; }
		Re::Map<Re::String, IniSectionItemPtr>* GetMap() override;
	private:
		Re::Map<Re::String, IniSectionItemPtr> Map{};
	};

	class IniSection
	{
	public:
		explicit IniSection(Re::String name) : Name(std::move(name)) {}

		const Re::String& GetName() const { return Name; }

		bool AddItem(const Re::String& itemName, IniSectionItemPtr ptr)
		{
			if(Properties.find(itemName) != Properties.end())
			{
				return false;
			}
			Properties.emplace(itemName, RE_MOVE(ptr));
			return true;
		}

		IniSectionItem* GetItem(const Re::String& itemName)
		{
			auto it = Properties.find(itemName);
			if (it != Properties.end())
			{
				return Re::UniquePtrGet(it->second);
			}
			return nullptr;
		}

	private:
		Re::String Name;
		Re::Map<Re::String, IniSectionItemPtr> Properties;
	};

class IniFile : public ICodeFile
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
			 return Re::UniquePtrGet(it->second);
		 }
		 return nullptr;
	 }

	 bool AddSection(const Re::String& name, IniSectionPtr&& Section)
	 {
		 if(Sections.find(name) != Sections.end())
		 {
			 return false;
		 }
		 Sections.emplace(name, RE_MOVE(Section));
		 return true;
	 }

		const Re::String& GetFilePath() const override { return FilePath; }
		const Re::String& GetContent() const override { return Content; }

private:
	Re::String FilePath;
	Re::String Content;
	Re::Map<Re::String, Re::UniquePtr<IniSection>> Sections;
};

}
