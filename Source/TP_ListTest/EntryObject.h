// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EntryObject.generated.h"

/**
 * î•ñ•Û—p‚ÌƒNƒ‰ƒX
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
