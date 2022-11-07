// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.


#include "AI/SKrakenCharacter.h"
#include "AI/SZombieAIController.h"
#include "Player/SCharacter.h"
#include "Player/SBaseCharacter.h"
#include "AI/SBotWaypoint.h"
#include "Player/SPlayerState.h"
/* AI Include */
#include "Perception/PawnSensingComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/NavMovementComponent.h"
#include "Components/AudioComponent.h"
#include "SurvivalGame/SurvivalGame.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


// Sets default values
ASKrakenCharacter::ASKrakenCharacter(const class FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	/* Note: We assign the Controller class in the Blueprint extension of this class 
		Because the zombie AIController is a blueprint in content and it's better to avoid content references in code.  */
	/*AIControllerClass = ASZombieAIController::StaticClass();*/

	///* Our sensing component to detect players by visibility and noise checks. */
	//PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));
	//PawnSensingComp->SetPeripheralVisionAngle(60.0f);
	//PawnSensingComp->SightRadius = 2000;
	//PawnSensingComp->HearingThreshold = 600;
	//PawnSensingComp->LOSHearingThreshold = 1200;

	///* Ignore this channel or it will absorb the trace impacts instead of the skeletal mesh */
	//GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);
	//GetCapsuleComponent()->SetCapsuleHalfHeight(96.0f, false);
	//GetCapsuleComponent()->SetCapsuleRadius(42.0f);

	///* These values are matched up to the CapsuleComponent above and are used to find navigation paths */
	//GetMovementComponent()->NavAgentProps.AgentRadius = 42;
	//GetMovementComponent()->NavAgentProps.AgentHeight = 192;

	//AudioLoopComp = CreateDefaultSubobject<UAudioComponent>(TEXT("ZombieLoopedSoundComp"));
	//AudioLoopComp->bAutoActivate = false;
	//AudioLoopComp->bAutoDestroy = false;
	//AudioLoopComp->SetupAttachment(RootComponent);

	//Health = 100;
	//MeleeDamage = 24.0f;
	//SprintingSpeedModifier = 3.0f;

	///* By default we will not let the AI patrol, we can override this value per-instance. */
	//BotType = EBotBehaviorType::Passive;
	//SenseTimeOut = 2.5f;

	/* Note: Visual Setup is done in the AI/ZombieCharacter Blueprint file */
}
