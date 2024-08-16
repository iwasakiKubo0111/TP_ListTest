// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/Image.h>
#include "EntryObject.h"
#include "ViewPortZone.generated.h"

class UListViewCPP;

/**
 * 
 */
UCLASS()
class TP_LISTTEST_API UViewPortZone : public UUserWidget
{
	GENERATED_BODY()
public:
	void SelectActorWithCtrlKey(AActor* actor, bool isCtrlKeyDown);
	void ForceSelectActor(AActor* actor, bool isSelect);
	void SelectListViewActorWithShiftKey(UEntryObject* entryObject);

	UListViewCPP* m_owningListViewInstance;

	TArray<AActor*> m_CurrentSelectedActors;
	TArray<AActor*> m_actors;
	TArray<UEntryObject*> m_entryObjects;
private:
	virtual void NativeConstruct()override;

	UFUNCTION(BlueprintCallable)
	void AddActor(AActor* actor, bool isMultiSelectState);

	UFUNCTION(BlueprintCallable)
	void RemoveActor(AActor* actor);

	UFUNCTION(BlueprintCallable)
	void RemoveCurrentSelectedActors();

	UFUNCTION(BlueprintCallable)
	void CopyCurrentSelectedActors();

	UFUNCTION(BlueprintCallable)
	void SelectAllActors(bool isSelect);

	bool SetActorOutLine(AActor* actor, bool visible);

	UFUNCTION()
	FEventReply OnImageClicked(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

	void SelectListViewActor(AActor* actor, bool isSelect);

	UEntryObject* FindEntryObjectByActor(AActor* actor);

	UPROPERTY(meta = (BindWidget))
	UImage* m_viewPortImage = nullptr;
	
};
