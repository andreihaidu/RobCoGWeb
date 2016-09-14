// Fill out your copyright notice in the Description page of Project Settings.

#include "RobCogWeb.h"
#include "RobCogWebGameMode.h"


ARobCogWebGameMode::ARobCogWebGameMode()
{

	PopUpMessage = FString(TEXT(""));
	EndLevelMessage = FString(TEXT(""));
}

void ARobCogWebGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateTextBoxes();

}

void ARobCogWebGameMode::BeginPlay()
{
	Super::BeginPlay();

	CurrentProgress = ELevelProgress::Playing;
	
	ThePlayer = Cast<AMyCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	//Bind the delegate to a function call
	if (ThePlayer)
	{
		ThePlayer->PopUp.AddDynamic(this, &ARobCogWebGameMode::PopUp);
		ThePlayer->Sub.AddDynamic(this, &ARobCogWebGameMode::Submit);
	}
}

void ARobCogWebGameMode::PopUp(FString Message)
{
	PopUpMessage = Message;

	GetWorld()->GetTimerManager().SetTimer(ResetPopUpTimer, this, &ARobCogWebGameMode::ResetPopUp , 3.f, false);
}

void ARobCogWebGameMode::Submit(FString Message, bool EndOrResume)
{
	EndLevelMessage = Message;

	if (EndOrResume)
	{
		if (CurrentProgress == ELevelProgress::Playing)
		{
			CurrentProgress = ELevelProgress::Finish;
		}
		else if (CurrentProgress == ELevelProgress::Finish)
		{
			CurrentProgress = ELevelProgress::Exit;
		}
	}
	else
	{
		CurrentProgress = ELevelProgress::Playing;
	}

}

void ARobCogWebGameMode::ResetPopUp()
{
	PopUpMessage = FString(TEXT(""));
}

void ARobCogWebGameMode::UpdateRightText(FString Message)
{
	DisplayMessageRight = Message;
}

void ARobCogWebGameMode::UpdateLeftText(FString Message)
{
	DisplayMessageLeft = Message;
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
				if (ThePlayer->AssetStateMap.Contains(ThePlayer->HighlightedActor))
				{
					UpdateLeftText(FString(TEXT("You can open and close drawers or doors.")));
					UpdateRightText(FString(TEXT("You need a free hand in order to do that!")));
				}

				else if (ThePlayer->ItemMap.Contains(ThePlayer->HighlightedActor))
				{
					UpdateLeftText(FString(TEXT("You can interract with items.\nPress click to pick up!")));
					if (ThePlayer->HighlightedActor->ActorHasTag(FName(TEXT("Stackable"))))
					{
						UpdateRightText(FString(TEXT("To pick stacks use Right Click.\nYou need two hands for that!")));
					}
				}
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

		if (CurrentProgress == ELevelProgress::Playing)
		{
			UpdateRightText(FString(TEXT("You can press 'P' at any time\nto check the hotkeys.")));
			UpdateLeftText(FString(TEXT("When the task is completed\npress 'O' to submit.")));
		}
		else
		{
			UpdateRightText(FString(TEXT("")));
			UpdateLeftText(FString(TEXT("")));
		}

		break;
	case ECurrentLevel::CleaningLevel:
		if (CurrentProgress == ELevelProgress::Playing)
		{
			UpdateRightText(FString(TEXT("You can press 'P' at any time\nto check the hotkeys.")));
			UpdateLeftText(FString(TEXT("When the task is completed\npress 'O' to submit.")));
		}
		else
		{
			UpdateRightText(FString(TEXT("")));
			UpdateLeftText(FString(TEXT("")));
		}
		break;
	}
	return;
}
