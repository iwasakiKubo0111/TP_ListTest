// Copyright Epic Games, Inc. All Rights Reserved.

#include "TP_ListTestGameMode.h"
#include "TP_ListTestCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATP_ListTestGameMode::ATP_ListTestGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
