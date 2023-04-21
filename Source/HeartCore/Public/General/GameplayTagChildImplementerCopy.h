#pragma once

#include "GameplayTagsManager.h"

// TODO THIS FILE IS COPIED FROM Plugins/Experimental/CommonUI/Source/CommonUI/Public/UITag.h
// IF EPIC EVER MOVES THIS STUFF INTO GAMEPLAY TAGS PROPERLY, THEN DELETE THIS AND USE THAT INSTEAD

/**
 * Implementation utility for typed subclasses of FGameplayTag.
 * Implemented here instead of directly within the macro to make debugging possible.
 */
template <typename TagT>
class TTypedTagStaticImplCopy
{
	friend TagT;

	static TagT AddNativeTag(const FString& TagBody)
	{
		if (!ensure(!TagBody.IsEmpty()))
		{
			return TagT();
		}

		FString TagStr;
		FString RootTagStr = FString::Printf(TEXT("%s."), TagT::GetRootTagStr());
		if (!TagBody.StartsWith(RootTagStr))
		{
			TagStr = RootTagStr + TagBody;
		}
		else
		{
			TagStr = TagBody;
#if !UE_BUILD_SHIPPING && !UE_BUILD_TEST
			ensureAlwaysMsgf(false, TEXT("Passed unnecessary prefix [%s] when creating a tag of type [%s] with the body [%s]"),
				*RootTagStr, TNameOf<TagT>::GetName(), *TagBody);
#endif
		}

		return UGameplayTagsManager::Get().AddNativeGameplayTag(FName(*TagStr));
	}

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	/** Intended for console commands/cheats: not for shipping code! */
	static FORCEINLINE TagT FindFromString_DebugOnly(const FString& PartialTagName)
	{
		return UGameplayTagsManager::Get().FindGameplayTagFromPartialString_Slow(PartialTagName);
	}
#endif

	static bool ExportTextItem(const TagT& Tag, FString& ValueStr, int32 PortFlags)
	{
		ValueStr += Tag.GetTagName().ToString();
		return true;
	}

	static TagT TryConvert(FGameplayTag VanillaTag, bool bChecked)
	{
		if (VanillaTag.MatchesTag(StaticImpl.RootTag))
		{
			return TagT(VanillaTag);
		}
		else if (VanillaTag.IsValid() && bChecked)
		{
			check(false);
		}
		return TagT();
	}

	TTypedTagStaticImplCopy()
	{
		LLM_SCOPE(ELLMTag::UI);
		UGameplayTagsManager::OnLastChanceToAddNativeTags().AddLambda([this]()
			{
				StaticImpl.RootTag = UGameplayTagsManager::Get().AddNativeGameplayTag(TagT::GetRootTagStr());
			});
	}
	TagT RootTag;
	static TTypedTagStaticImplCopy StaticImpl;
};

template <typename TagT>
TTypedTagStaticImplCopy<TagT> TTypedTagStaticImplCopy<TagT>::StaticImpl;

// Intended to be the absolute last thing in the definition of a UI tag
#define END_HEART_TAG_DECL(TagType, TagRoot)	\
public:	\
	TagType() {}	\
	static TagType GetRootTag() { return TTypedTagStaticImplCopy<TagType>::StaticImpl.RootTag; }	\
	static TagType TryConvert(FGameplayTag FromTag) { return TTypedTagStaticImplCopy<TagType>::TryConvert(FromTag, false); }	\
	static TagType ConvertChecked(FGameplayTag FromTag) { return TTypedTagStaticImplCopy<TagType>::TryConvert(FromTag, true); }	\
	static TagType AddNativeTag(const FString& TagBody) { return TTypedTagStaticImplCopy<TagType>::AddNativeTag(TagBody); }	\
	bool ExportTextItem(FString& ValueStr, const TagType& DefaultValue, UObject* Parent, int32 PortFlags, UObject* ExportRootScope) const	\
	{	\
		return TTypedTagStaticImplCopy<TagType>::ExportTextItem(*this, ValueStr, PortFlags);	\
	}	\
protected:	\
	TagType(FGameplayTag Tag) { TagName = Tag.GetTagName(); }	\
	static const TCHAR* GetRootTagStr() { return TagRoot; }	\
	friend class TTypedTagStaticImplCopy<TagType>;	\
};	\
Expose_TNameOf(TagType)	\
template<>	\
struct TStructOpsTypeTraits<TagType> : public TStructOpsTypeTraitsBase2<TagType>	\
{	\
	enum	\
	{	\
		WithExportTextItem = true,	\
		WithImportTextItem = true	\
	};