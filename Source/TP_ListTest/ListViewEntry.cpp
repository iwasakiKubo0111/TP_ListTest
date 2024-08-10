// Fill out your copyright notice in the Description page of Project Settings.


#include "ListViewEntry.h"
#include "EntryObject.h"
#include <Kismet/KismetSystemLibrary.h>

void UListViewEntry::NativeConstruct()
{
	Super::NativeConstruct();

	if (m_expandableArea)
	{
		m_expandableArea->OnExpansionChanged.AddDynamic(this, &UListViewEntry::OnExpandableAreaChanged);
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
		m_expandableArea->SetIsExpanded(data->m_IsExpand);

	}
}

void UListViewEntry::OnExpandableAreaChanged(UExpandableArea* Area, bool bIsExpanded)
{
	if (m_owningEntryObjectInstance)
	{
		FString BoolString = bIsExpanded ? TEXT("true") : TEXT("false");
		FText Text = m_owningEntryObjectInstance->m_headerText;

		UKismetSystemLibrary::PrintString(this, Text.ToString() + BoolString, true, true, FLinearColor::Green, 5.0f);
		
		m_owningEntryObjectInstance->m_IsExpand = bIsExpanded;
	}
}
