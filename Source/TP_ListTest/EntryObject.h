// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EntryObject.generated.h"

class UViewPortZone;

/**
 * èÓïÒï€éùópÇÃÉNÉâÉX
 */
UCLASS()
class TP_LISTTEST_API UEntryObject : public UObject
{
	GENERATED_BODY()
public:
	AActor* m_owningActor;
	UViewPortZone* m_owningViewPortZoneInstance;
	FText m_headerText;
	FText m_bodyText;
	bool m_isExpand;
	bool m_isSelect;
};
