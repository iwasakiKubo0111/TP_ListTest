// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/ScrollBox.h>
#include "ViewPortZone.h"
#include <Components\ListView.h>
#include "ListViewCPP.generated.h"

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
	void FilterActorsWithMeshes(TArray<AActor*>& OutActors);
	UViewPortZone* m_viewPortZone;

	UPROPERTY(meta = (BindWidget))
	UListView* m_listView = nullptr;
protected:
	virtual FReply  NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
private:
	
};
