// Fill out your copyright notice in the Description page of Project Settings.


#include "ListViewEntry.h"
#include "EntryObject.h"
#include <Kismet/KismetSystemLibrary.h>

FSlateColor BlueColor(FLinearColor::Blue);
FSlateColor WhiteColor(FLinearColor::White);

void UListViewEntry::NativeConstruct()
{
	Super::NativeConstruct();

	if (m_expandableArea)
	{
		m_expandableArea->OnExpansionChanged.AddDynamic(this, &UListViewEntry::OnExpandableAreaChanged);
	}

	if (m_headerButton)
	{
		m_headerButton->OnClicked.AddDynamic(this, &UListViewEntry::OnButtonClick_header);
	}

}

void UListViewEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	UEntryObject* data = Cast<UEntryObject>(ListItemObject);
	if (data)
	{
		m_owningEntryObjectInstance = data;
		m_headerText->SetText(data->m_headerText);
		m_bodyText->SetText(data->m_bodyText);
		m_expandableArea->SetIsExpanded(data->m_isExpand);

		if (data->m_isSelect)
		{
			m_headerText->SetColorAndOpacity(BlueColor);
		}
		else
		{
			m_headerText->SetColorAndOpacity(WhiteColor);
		}
	}
}

void UListViewEntry::OnButtonClick_header()
{
	//ctrlƒL[‚ð‰Ÿ‚µ‚Ä‚¢‚é‚©‚Ç‚¤‚©
	const FModifierKeysState& ModifierKeys = FSlateApplication::Get().GetModifierKeys();
	if (ModifierKeys.IsLeftControlDown() || ModifierKeys.IsRightControlDown() || ModifierKeys.IsShiftDown())
	{
		m_owningEntryObjectInstance->m_owningViewPortZoneInstance->SelectActorWithCtrlKey(m_owningEntryObjectInstance->m_owningActor, true);
	}
	else
	{
		m_owningEntryObjectInstance->m_owningViewPortZoneInstance->SelectActorWithCtrlKey(m_owningEntryObjectInstance->m_owningActor, false);
	}

	if (ModifierKeys.IsShiftDown())
	{
		m_owningEntryObjectInstance->m_owningViewPortZoneInstance->SelectListViewActorWithShiftKey(m_owningEntryObjectInstance);
	}

}

void UListViewEntry::OnExpandableAreaChanged(UExpandableArea* Area, bool bIsExpanded)
{
	if (m_owningEntryObjectInstance)
	{
		FString BoolString = bIsExpanded ? TEXT("true") : TEXT("false");
		FText Text = m_owningEntryObjectInstance->m_headerText;

		UKismetSystemLibrary::PrintString(this, Text.ToString() + BoolString, true, true, FLinearColor::Green, 5.0f);
		
		m_owningEntryObjectInstance->m_isExpand = bIsExpanded;
	}
}

