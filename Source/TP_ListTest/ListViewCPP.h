// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/ScrollBox.h>
#include "ListViewCPP.generated.h"

class UListView;
class UUserWidget;

/**
 * 
 */
UCLASS()
class TP_LISTTEST_API UListViewCPP : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
private:
	UPROPERTY(meta = (BindWidget))
	UListView* m_listView = nullptr;
};
