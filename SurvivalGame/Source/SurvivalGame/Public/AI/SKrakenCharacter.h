// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Player/SBaseCharacter.h"
#include "AI/SZombieCharacter.h"
#include "SKrakenCharacter.generated.h"

class USoundCue;

UCLASS(ABSTRACT)
class SURVIVALGAME_API ASKrakenCharacter : public ASZombieCharacter
{
	GENERATED_BODY()

public:

	ASKrakenCharacter(const class FObjectInitializer& ObjectInitializer);
};
