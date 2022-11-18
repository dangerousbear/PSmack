// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#include "World/SCoopGameMode.h"
#include "NavigationSystem.h"
#include "Player/SPlayerState.h"
#include "Player/SCharacter.h"
#include "World/SGameState.h"
#include "EngineUtils.h"
#include "Player/SPlayerController.h"
#include <Kismet/GameplayStatics.h>
#include <AI/SZombieCharacter.h>



ASCoopGameMode::ASCoopGameMode()
{
	/* Disable damage to coop buddies  */
	bAllowFriendlyFireDamage = false;
	bSpawnAtTeamPlayer = true;
  bUseSeamlessTravel = true;
	
	ScoreNightSurvived = 1000;
}


/*
	RestartPlayer - Spawn the player next to his living coop buddy instead of a PlayerStart
*/
void ASCoopGameMode::RestartPlayer(class AController* NewPlayer)
{
  RestartPlayerInner(NewPlayer);
	if (auto Pawn = Cast<ASCharacter>(NewPlayer->GetPawn())) {
    Pawn->InitState();
  }
}

void ASCoopGameMode::RestartPlayerInner(class AController* NewPlayer) {
  /* Fallback to PlayerStart picking if team spawning is disabled or we're trying to spawn a bot. */
  if (!bSpawnAtTeamPlayer || (NewPlayer->PlayerState && NewPlayer->PlayerState->IsABot()))
  {
    Super::RestartPlayer(NewPlayer);
    return;
  }

	/* Look for a live player to spawn next to */
	FVector SpawnOrigin = FVector::ZeroVector;
	FRotator StartRotation = FRotator::ZeroRotator;
	for (TActorIterator<APawn> It(GetWorld()); It; ++It)
	{
		ASCharacter* MyCharacter = Cast<ASCharacter>(*It);
		if (MyCharacter && MyCharacter->IsAlive())
		{
			/* Get the origin of the first player we can find */
			SpawnOrigin = MyCharacter->GetActorLocation();
			StartRotation = MyCharacter->GetActorRotation();
			break;
		}
	}

	/* No player is alive (yet) - spawn using one of the PlayerStarts */
	if (SpawnOrigin == FVector::ZeroVector)
	{
		Super::RestartPlayer(NewPlayer);
		return;
	}

	/* Get a point on the nav mesh near the other player */
	FNavLocation StartLocation;
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetNavigationSystem(this);
	if (NavSystem && NavSystem->GetRandomPointInNavigableRadius(SpawnOrigin, 250.0f, StartLocation))
	{
		// Try to create a pawn to use of the default class for this player
		if (NewPlayer->GetPawn() == nullptr && GetDefaultPawnClassForController(NewPlayer) != nullptr)
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.Instigator = GetInstigator();
			APawn* ResultPawn = GetWorld()->SpawnActor<APawn>(GetDefaultPawnClassForController(NewPlayer), StartLocation.Location, StartRotation, SpawnInfo);
			if (ResultPawn == nullptr)
			{
				UE_LOG(LogGameMode, Warning, TEXT("Couldn't spawn Pawn of type %s at %s"), *GetNameSafe(DefaultPawnClass), &StartLocation.Location);
			}
			NewPlayer->SetPawn(ResultPawn);
		}

		if (NewPlayer->GetPawn() == nullptr)
		{
			NewPlayer->FailedToSpawnPawn();
		}
		else
		{
			NewPlayer->Possess(NewPlayer->GetPawn());

			// If the Pawn is destroyed as part of possession we have to abort
			if (NewPlayer->GetPawn() == nullptr)
			{
				NewPlayer->FailedToSpawnPawn();
			}
			else
			{
				// Set initial control rotation to player start's rotation
				NewPlayer->ClientSetRotation(NewPlayer->GetPawn()->GetActorRotation(), true);

				FRotator NewControllerRot = StartRotation;
				NewControllerRot.Roll = 0.f;
				NewPlayer->SetControlRotation(NewControllerRot);

				SetPlayerDefaults(NewPlayer->GetPawn());
			}
		}
	}
}


void ASCoopGameMode::OnNightEnded()
{
	/* Respawn spectating players that died during the night */
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		/* Look for all players that are spectating */
		ASPlayerController* MyController = Cast<ASPlayerController>(*It);
		if (MyController)
		{
			if (MyController->PlayerState->IsSpectator())
			{
				RestartPlayer(MyController);
				MyController->ClientHUDStateChanged(EHUDState::Playing);
			}
			else
			{
				/* Player still alive, award him some points */
				ASCharacter* MyPawn = Cast<ASCharacter>(MyController->GetPawn());
				if (MyPawn && MyPawn->IsAlive())
				{
					MyPawn->IncrementXP(100.0);
					ASPlayerState* PS = Cast<ASPlayerState>(MyController->PlayerState);
					if (PS)
					{
						PS->ScorePoints(ScoreNightSurvived);
					}
				}
			}
		}
	}
}


void ASCoopGameMode::Killed(AController* Killer, AController* VictimPlayer, APawn* VictimPawn, const UDamageType* DamageType)
{
	ASPlayerState* KillerPS = Killer ? Cast<ASPlayerState>(Killer->PlayerState) : nullptr;
	ASPlayerState* VictimPS = VictimPlayer ? Cast<ASPlayerState>(VictimPlayer->PlayerState) : nullptr;

	if (KillerPS && KillerPS != VictimPS && !KillerPS->IsABot())
	{
		KillerPS->AddKill();
		KillerPS->ScorePoints(10);

		for (TActorIterator<APawn> It(GetWorld()); It; ++It)
		{
			if (auto Player = Cast<ASCharacter>(*It)) {
				Player->IncrementXP(8.0);
			}
		}
	}

	if (VictimPS && !VictimPS->IsABot())
	{
		VictimPS->AddDeath();
	}

	/* End match is all players died */
	CheckMatchEnd();
}


void ASCoopGameMode::CheckMatchEnd()
{
	bool bHasAlivePlayer = false;
	for (TActorIterator<APawn> It(GetWorld()); It; ++It)
	{
		ASCharacter* MyPawn = Cast<ASCharacter>(*It);
		if (MyPawn && MyPawn->IsAlive())
		{
			ASPlayerState* PS = Cast<ASPlayerState>(MyPawn->GetPlayerState());
			if (PS)
			{
				if (!PS->IsABot())
				{
					/* Found one player that is still alive, game will continue */
					bHasAlivePlayer = true;
					break;
				}
			}
		}
	}

	/* End game is all players died */
	if (!bHasAlivePlayer)
	{
		FinishMatch();
	}
}


void ASCoopGameMode::FinishMatch()
{
	if (IsMatchInProgress())
	{

		//EndMatch();

		/* Stop spawning bots */
		//GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawns);

		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
		{
			ASPlayerController* MyController = Cast<ASPlayerController>(*It);
			if (MyController)
			{
				MyController->ClientHUDStateChanged(EHUDState::MatchEnd);
			}
		}
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ResetTimer, this, &ASCoopGameMode::ResetMatch, 1.0, false);
	}
}

void ASCoopGameMode::RespawnPlayers() {
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		/* Look for all players that are spectating */
		ASPlayerController* MyController = Cast<ASPlayerController>(*It);
		if (MyController && MyController->PlayerState->IsSpectator())
		{
			RestartPlayer(MyController);
			MyController->ClientHUDStateChanged(EHUDState::Playing);
		}
	}
}


void ASCoopGameMode::ResetMatch() {
	for (TActorIterator<APawn> It(GetWorld()); It; ++It)
	{
		if (auto Pawn = Cast<ASBaseCharacter>(*It)) {
			Pawn->Destroy();
		}
	}
  for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
  {
    if (auto MyController = Cast<ASPlayerController>(*It))
    {
      RestartPlayer(MyController);
      MyController->ClientHUDStateChanged(EHUDState::Playing);
    }
  }


	if (auto MyGameState = Cast<ASGameState>(GameState))
	{
		MyGameState->ElapsedGameMinutes = TimeOfDayStart;
	}
  DayIndex = 0;













  //Store state, reset, set state
  //struct SavedState {
  //  int32 PlayerTypeIndex;
  //  int32 PlayerLevel;
  //  int32 PlayerSkillPointsAvailable;
  //  TArray<int32> PlayerTalentLevels;
  //  float PlayerXP;
  //};

  //auto states = std::vector<SavedState>();

  //for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
  //{
  //  if (auto MyController = Cast<ASPlayerController>(*It))
  //  {
  //    if (auto PS = Cast<ASPlayerState>(MyController->PlayerState)) {
  //      states.push_back(SavedState{
  //      PS->PlayerTypeIndex,
  //      PS->PlayerLevel,
  //      PS->PlayerSkillPointsAvailable,
  //      PS->PlayerTalentLevels,
  //      PS->PlayerXP
  //        });
  //    }
  //  }
  //}

  //size_t i = 0;
	//GetWorld()->ServerTravel("/Game/Maps/Landscape_Map");
	//ResetLevel();
	//for (FActorIterator It(GetWorld()); It; ++It)
	//{
	//	auto A = *It;
	//	if (A && !A->IsPendingKill() && A != this && !A->IsA<AController>() && !A->IsA<ASCharacter>())
	//	{
	//		A->Reset();
	//	}
	//}
	//for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
 // //for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It && i < states.size(); It++)
 // {
 //   if (auto MyController = Cast<ASPlayerController>(*It))
 //   {
	//		RestartPlayer(MyController);
	//		MyController->ClientHUDStateChanged(EHUDState::Playing);
	//		//if (auto MyPawn = Cast<ASCharacter>(MyController->GetPawn())) {
	//		//	MyPawn->InitState();
	//		//}
 //     //if (auto PS = Cast<ASPlayerState>(MyController->PlayerState)) {
 //       //PS->PlayerTypeIndex = states[i].PlayerTypeIndex;
 //       //PS->PlayerLevel = states[i].PlayerLevel;
 //       //PS->PlayerSkillPointsAvailable = states[i].PlayerSkillPointsAvailable;
 //       //PS->PlayerTalentLevels = states[i].PlayerTalentLevels;
 //       //PS->PlayerXP = states[i].PlayerXP;
 //       
 //       //++i;
 //     //}
 //   }
 // }
	//bReadyToStart = false;
	////SetMatchState(MatchState::WaitingToStart);
	//StartMatch();
}

