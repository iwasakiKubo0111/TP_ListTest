// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/TextBlock.h>
#include "Blueprint/IUserObjectListEntry.h"
#include <Components/ExpandableArea.h>
#include "ListViewEntry.generated.h"

class UEntryObject;

/**
 * 
 */
UCLASS()
class TP_LISTTEST_API UListViewEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UExpandableArea* m_expandableArea;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* m_headerText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* m_bodyText;

	UEntryObject* m_owningEntryObjectInstance;

	UFUNCTION()
	void OnExpandableAreaChanged(UExpandableArea* Area, bool bIsExpanded);

private:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;


};
