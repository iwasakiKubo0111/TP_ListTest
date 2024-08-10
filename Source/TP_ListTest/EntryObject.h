// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EntryObject.generated.h"

/**
 * 情報保持用のクラス
 */
UCLASS()
class TP_LISTTEST_API UEntryObject : public UObject
{
	GENERATED_BODY()
public:
	FText m_headerText;
	FText m_bodyText;
	bool m_IsExpand;
};
