// Fill out your copyright notice in the Description page of Project Settings.

#include "RobCogWeb.h"
#include "RobCogWebGameMode.h"


ARobCogWebGameMode::ARobCogWebGameMode()
{
	//Set default values for the in game messages
	OneHandOcupied = FString(TEXT("Can't pick up with two hands\nif they are not both empty!"));
	SmthOnTop = FString(TEXT("Make sure it does not have anything\non top before picking!"));
	ActionNotValid = FString(TEXT("Action not valid!"));
	GetCloser = FString(TEXT("You need to get closer!"));
}

void ARobCogWebGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateTextBoxes();

	//Section to update in game pop-ups
	if (ThePlayer)
	{

	}
}

void ARobCogWebGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	ThePlayer = Cast<AMyCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
}

void ARobCogWebGameMode::UpdateRightText(FString Message)
{
	DisplayMessageRight = Message;
}

void ARobCogWebGameMode::UpdateLeftText(FString Message)
{
	DisplayMessageLeft = Message;
}

UFUNCTION(BlueprintImplementableEvent, Category = "Display")
void ARobCogWebGameMode::UpdateTooltip(FString Message)
{
	PopUpMessage = Message;
}

void ARobCogWebGameMode::UpdateTextBoxes()
{
	switch (LevelName)
	{
	case ECurrentLevel::TutorialLevel :
		//Help text to display at the beginig of the game
		UpdateLeftText(FString(TEXT("Use W,A,S,D keys to move around the kitchen.\nFeel free to look anywhere and try anything!")));
		UpdateRightText(FString(TEXT("You can use BOTH your hands! \nUse TAB to switch between them!")));

		if (ThePlayer)
		{
			//Display message when focused on an interractive item
			if (ThePlayer->HighlightedActor)
			{
				UpdateLeftText(FString(TEXT("You can interract with items.\nPress click to see what happens!")));
				UpdateRightText(FString(TEXT("To pick stacks use Right Click.\nYou need two hands for that!")));
			}

			//Rotation and positioning adjustment messages
			if (ThePlayer->SelectedObject && !ThePlayer->TwoHandSlot.Num())
			{
				UpdateLeftText(FString(TEXT("You can adjust the position with arrow keys.\nYou can press 'R' to enter rotation mode!")));
				if (ThePlayer->RotationAxisIndex)
				{
					UpdateRightText(FString(TEXT("Press 'R' to switch between axis.\nUse the mouse wheel to adjust rotation.")));
				}
			}

			//Message when the user holds stacks
			if (ThePlayer->TwoHandSlot.Num())
			{
				UpdateLeftText(FString(TEXT("You can create more stacks by placing \nthe same type of items on top of eachother")));
				UpdateRightText(FString(TEXT("We don't want to overwork the robot.\nYou can only pick a limited\nnumber of items at once!")));
			}
		}
		break;

	case ECurrentLevel::BreakfastLevel:
		UpdateRightText(FString(TEXT("You can press 'P' at any time\nto check the hotkeys.")));
		break;
	case ECurrentLevel::CleaningLevel:
		UpdateRightText(FString(TEXT("You can press 'P' at any time\nto check the hotkeys.")));
		break;
	}
	return;
}
