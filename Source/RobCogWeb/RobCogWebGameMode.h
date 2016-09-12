// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MyCharacter.h"
#include "GameFramework/GameMode.h"
#include "RobCogWebGameMode.generated.h"

//Enum used in the TMap which keeps the state of the drawer
UENUM(BlueprintType)
enum class ECurrentLevel : uint8
{
	TutorialLevel UMETA(DisplayName = "TutorialLevel"),
	BreakfastLevel UMETA(DisplayName = "BreakfastLevel"),
	CleaningLevel UMETA(DisplayName = "CleaningLevel"),
	Unknown UMETA(DisplayName = "Unkown")
};

/**
 * 
 */
UCLASS()
class ROBCOGWEB_API ARobCogWebGameMode : public AGameMode
{
	GENERATED_BODY()
	
protected:
	//Variable to store our character
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	AMyCharacter* ThePlayer;

	//String used for displaying help messages for the user
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString DisplayMessageLeft;

	//String used for displaying help messages for the user
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString DisplayMessageRight;

	//String used for displaying help messages for the user
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString PopUpMessage;

	//Some variables to store pop up messages
	FString OneHandOcupied;
	FString SmthOnTop;
	FString ActionNotValid;
	FString GetCloser;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ECurrentLevel LevelName;
	
public:
	//Constructor for the game mode class
	ARobCogWebGameMode();

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Setter functions for the Display Message varraibles
	void UpdateRightText(FString Message);
	void UpdateLeftText(FString Message);

	UFUNCTION(BlueprintImplementableEvent, Category = "Display")
	void UpdateTooltip(FString Message);

	//Function which handles the logic of the display messages
	void UpdateTextBoxes();

};
