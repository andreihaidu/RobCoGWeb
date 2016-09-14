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

UENUM(BlueprintType)
enum class ELevelProgress : uint8
{
	//State in which the game starts
	Playing UMETA(DisplayName = "Playing"),
	//State accessed when pressed 'O' (the key assigned for submiting and finishing the game)
	Finish UMETA(DisplayName = "Finish"),
	//State which exits the level reached by pressing 'O' again when in Finish state
	Exit UMETA(DisplayName = "Exit")

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

	//String used for displaying end level text
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString EndLevelMessage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ECurrentLevel LevelName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ELevelProgress CurrentProgress;
	
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

	//Function which handles the logic of the display messages
	void UpdateTextBoxes();

	//Function to respond to the pop-up delegate sent by the character class
	UFUNCTION(BlueprintCallable, Category = "Interface")
	void PopUp(FString Message);

	//Function to respond to the submit progrss delegate sent by the character class
	UFUNCTION(BlueprintCallable, Category = "Interface")
	void Submit(FString Message, bool EndOrResume);

	//Function to reset popup text
	void ResetPopUp();

	//Timer for reseting the pop up text
	FTimerHandle ResetPopUpTimer;

};
