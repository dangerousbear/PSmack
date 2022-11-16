// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <vector>
#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class SURVIVALGAME_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()

	ASPlayerState();

	UPROPERTY(Transient, Replicated)
	int32 NumKills;

	UPROPERTY(Transient, Replicated)
	int32 NumDeaths;

	/* Team number assigned to player */
	UPROPERTY(Transient, Replicated)
	int32 TeamNumber;

	virtual void Reset() override;

public: 

	void AddKill();

	void AddDeath();

	void ScorePoints(int32 Points);

	void SetTeamNumber(int32 NewTeamNumber);

	UFUNCTION(BlueprintCallable, Category = "Teams")
	int32 GetTeamNumber() const;

	UFUNCTION(BlueprintCallable, Category = "Score")
	int32 GetKills() const;

	UFUNCTION(BlueprintCallable, Category = "Score")
	int32 GetDeaths() const;

	UFUNCTION(BlueprintCallable, Category = "Player")
	void SetPlayerTypeIndex(int32 Index);

	void CopyProperties(APlayerState* PlayerState) override;

  UPROPERTY(Transient, Replicated)
    int32 PlayerTypeIndex;
  UPROPERTY(Transient, Replicated)
    int32 PlayerLevel;
  UPROPERTY(Transient, Replicated)
    int32 PlayerSkillPointsAvailable;
  UPROPERTY(Transient, Replicated)
    TArray<int32> PlayerTalentLevels;
  UPROPERTY(Transient, Replicated)
    float PlayerXP;
};
