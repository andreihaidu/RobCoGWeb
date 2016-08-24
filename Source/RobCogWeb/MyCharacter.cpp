// Fill out your copyright notice in the Description page of Project Settings.

#include "RobCogWeb.h"
#include "MyCharacter.h"
#include "GameFramework/InputSettings.h"


// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set this pawn to be controlled by the lowest-numbered player
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(5.f, 80.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	MyCharacterCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MyCharacterCamera"));
	MyCharacterCamera->SetupAttachment(GetCapsuleComponent());
	MyCharacterCamera->RelativeLocation = FVector(0.f, 0.f, 64.f); // Position the camera
	MyCharacterCamera->bUsePawnControlRotation = true;

	//Set the value of the applied force
	AppliedForce = FVector(1000, 1000, 1000);

	//Initialize TraceParams parameter
	TraceParams = FCollisionQueryParams(FName(TEXT("Trace")), true, this);
	TraceParams.bTraceComplex = true;
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;

	//Set the maximum grasping length
	MaxGraspLength = 200.f;

	//Set the pointers to the items held in hands to null at the begining of the game
	LeftHandSlot = nullptr;
	RightHandSlot = nullptr;

	//Set the rotations to null
	LeftHandRotator = FRotator(0.f, 0.f, 0.f);
	RightHandRotator = FRotator(0.f, 0.f, 0.f);

	//By default our character will perfom actions with the right hand first
	bRightHandSelected = true;

	//At the begining we don't hold an item which can be rotated
	bRotationModeAllowed = false;

	// 1 - Z axis ; 2 - X axis ; 3 - Y axis ; 0 - default rotation disabled
	RotationAxisIndex = 0;

	//Offsets for positioning the items held in hand
	RightZPos = 30.f;
	LeftZPos = 30.f;
	RightYPos = 20;
	LeftYPos = 20;
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//Gets all actors in the world, used for identifying our drawers and setting their initial state to closed
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), AllActors);

	//Loop that checks if current object matches our drawer
	for (const auto ActorIt : AllActors)
	{
		//Command used for determining the exact name of an actor. Only useful when designing.
		//UE_LOG(LogTemp, Warning, TEXT("Actor name: %s"), *ActorIt->GetName());

		//Finds the actors for the Handles, used to set the initial state of our drawers to closed 
		if (ActorIt->GetName().Contains("Handle"))
		{
			if (GetStaticMesh(ActorIt) != nullptr)
			{
				if (!ActorIt->GetName().Contains("Door"))
				{
					GetStaticMesh(ActorIt)->AddImpulse(-1 * AppliedForce * ActorIt->GetActorForwardVector());
				}
				AssetStateMap.Add(ActorIt->GetAttachParentActor(), EAssetState::Closed);
			}
		}
		//Remember to tag items when adding them into the world
		//Or CREATE a function which does that automatically
		else if (ActorIt->ActorHasTag(FName(TEXT("Item"))))
		{
			ItemMap.Add(ActorIt, EItemType::GeneralItem);
		}
	}
}

UStaticMeshComponent* AMyCharacter::GetStaticMesh(AActor* Actor)
{
	for (auto Component : Actor->GetComponents())
	{
		if (Component->GetName().Contains("StaticMeshComponent"))
		{
			return Cast<UStaticMeshComponent>(Component);
		}
	}
	return nullptr;
}

void AMyCharacter::OpenCloseAction(AActor* OpenableActor)
{
	if (OpenableActor->GetName().Contains("Handle"))
	{
		OpenableActor = OpenableActor->GetAttachParentActor();
	}

	if (!AssetStateMap.Contains(OpenableActor))
	{
		return;
	}

	else
	{
		if (AssetStateMap.FindRef(OpenableActor) == EAssetState::Closed)
		{
			GetStaticMesh(OpenableActor)->AddImpulse(AppliedForce * OpenableActor->GetActorForwardVector());
			AssetStateMap.Add(OpenableActor, EAssetState::Open);
		}
		else if (AssetStateMap.FindRef(OpenableActor) == EAssetState::Open)
		{
			GetStaticMesh(OpenableActor)->AddImpulse(-AppliedForce * OpenableActor->GetActorForwardVector());
			AssetStateMap.Add(OpenableActor, EAssetState::Closed);
		}
		return;
	}
}

// Called every frame
void AMyCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	//Update tooltips
	UpdateTextBoxes();

	//Draw a straight line in front of our character
	Start = MyCharacterCamera->GetComponentLocation();
	End = Start + MyCharacterCamera->GetForwardVector()*MaxGraspLength;
	HitObject = FHitResult(ForceInit);
	GetWorld()->LineTraceSingleByChannel(HitObject, Start, End, ECC_Pawn, TraceParams);

	//Mouse hovered behaviour with an empty hand
	if (!SelectedObject)
	{
		//Turn off the highlight effect when changing to another actor
		if (HighlightedActor && HitObject.GetActor() != HighlightedActor)
		{
			GetStaticMesh(HighlightedActor)->SetRenderCustomDepth(false);
			HighlightedActor = nullptr;
		}

		//Check if there is an object blocking the hit and if it is in our hand's range
		if (HitObject.bBlockingHit && HitObject.Distance < MaxGraspLength)
		{
			//Check if the object has interractive behaviour enabled
			if (AssetStateMap.Contains(HitObject.GetActor()) || AssetStateMap.Contains(HitObject.GetActor()->GetAttachParentActor()) || ItemMap.Contains(HitObject.GetActor()))
			{
				HighlightedActor = HitObject.GetActor();
				GetStaticMesh(HighlightedActor)->SetRenderCustomDepth(true);
			}
		}
	}

	//Behaviour for selected object in hand
	else
	{
		//Turn off the highlight effect because we can't pick up with this hand.
		if (HighlightedActor)
		{
			GetStaticMesh(HighlightedActor)->SetRenderCustomDepth(false);
			HighlightedActor = nullptr;
		}

		//Enable the player to access rotation mode
		bRotationModeAllowed = true;

	}

	//Draw object from the right hand
	if (RightHandSlot)
	{
		RightHandSlot->SetActorRotation(RightHandRotator + FRotator(0.f, GetActorRotation().Yaw, 0.f));
		GetStaticMesh(RightHandSlot)->SetWorldLocation(GetActorLocation() + FVector(20.f, 20.f, 20.f) * GetActorForwardVector() + FVector(RightYPos, RightYPos, RightYPos) * GetActorRightVector() + FVector(0.f, 0.f, RightZPos));
	}

	//Draw object from the left hand
	if (LeftHandSlot)
	{
		LeftHandSlot->SetActorRotation(LeftHandRotator + FRotator(0.f, GetActorRotation().Yaw, 0.f));
		GetStaticMesh(LeftHandSlot)->SetWorldLocation(GetActorLocation() + FVector(20.f, 20.f, 20.f) * GetActorForwardVector() - FVector(LeftYPos, LeftYPos, LeftYPos) * GetActorRightVector() + FVector(0.f, 0.f, LeftZPos));
	}
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	//// Default Camera view bindings
	InputComponent->BindAxis("LookUp", this, &AMyCharacter::AddControllerPitchInput);
	InputComponent->BindAxis("Turn", this, &AMyCharacter::AddControllerYawInput);

	//// Respond every frame to the values of our two movement axes, "MoveX" and "MoveY".
	InputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);

	//Set up the input from the mouse
	InputComponent->BindAction("Click", IE_Pressed, this, &AMyCharacter::Click);

	//Input from the tab button which switches selected hand
	InputComponent->BindAction("SwitchSelectedHand", IE_Pressed, this, &AMyCharacter::SwitchSelectedHand);

	//Key to switch between the axis the item turns around
	InputComponent->BindAction("SwitchRotationAxis", IE_Pressed, this, &AMyCharacter::SwitchRotationAxis);

	//Input from the mouse wheel to rotate the object
	InputComponent->BindAxis("RotateObject", this, &AMyCharacter::RotateObject);

	//Input frome keyboard arrows
	InputComponent->BindAxis("MoveItemZ", this, &AMyCharacter::MoveItemZ);
	InputComponent->BindAxis("MoveItemY", this, &AMyCharacter::MoveItemY);
}

void AMyCharacter::MoveForward(const float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// Find out which way is forward
		FRotator Rotation = Controller->GetControlRotation();
		// Limit pitch when walking or falling
		if (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling())
		{
			Rotation.Pitch = 0.0f;
		}
		// add movement in that direction
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
		AddMovementInput(Direction, Value *0.5);
	}
}

void AMyCharacter::MoveRight(const float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value *0.5);
	}
}

void AMyCharacter::SwitchSelectedHand()
{
	bRightHandSelected = !bRightHandSelected;
	if (bRightHandSelected)
	{
		SelectedObject = RightHandSlot;
		UE_LOG(LogTemp, Warning, TEXT("Our character will perform the next action with his RIGHT hand"));
	}
	else
	{
		SelectedObject = LeftHandSlot;
		UE_LOG(LogTemp, Warning, TEXT("Our character will perform the next action with his LEFT hand"));
	}

	//Exit rotation mode
	RotationAxisIndex = 0;
}

void AMyCharacter::Click()
{
	//Behaviour when we want to drop the item currently held in hand
	if (SelectedObject && HitObject.Distance < MaxGraspLength)
	{
		//Drops our currently selected item on the surface clicked on
		DropFromInventory(SelectedObject, HitObject);
	}

	//Behaviour when wanting to grab an item or opening/closing actions
	else if (HighlightedActor)
	{
		//Section for items that can be picked up and moved around
		if (ItemMap.Contains(HighlightedActor))
		{
			//Picks up the item selected
			SelectedObject = HighlightedActor;
			PickToInventory(SelectedObject);
		}

		//Section for openable actors
		else
		{
			OpenCloseAction(HighlightedActor);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Not a valid action!"));
		return;
	}
}

void AMyCharacter::PickToInventory(AActor* CurrentObject)
{
	//Add a reference and an icon of the object in the correct item slot (left or right hand) and save the value of our rotator
	if (bRightHandSelected)
	{
		RightHandSlot = CurrentObject;
		RightHandRotator = RightHandSlot->GetActorRotation();
	}
	else
	{
		LeftHandSlot = CurrentObject;
		LeftHandRotator = LeftHandSlot->GetActorRotation();
	}

	//Set collision to overlap other actors, we set up the GameTraceChannel1 to our custom overlapping collision.
	//GetStaticMesh(CurrentObject)->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);
	//Deactivate the gravity
	GetStaticMesh(CurrentObject)->SetEnableGravity(false);

	//Ignore clicking on item if held in hand
	TraceParams.AddIgnoredComponent(GetStaticMesh(CurrentObject));
}

void AMyCharacter::DropFromInventory(AActor* CurrentObject, FHitResult HitSurface)
{
	if (HitSurface.Distance > MaxGraspLength)
	{
		return;
	}
	//Find the bounding limits of the currently selected object 
	GetStaticMesh(CurrentObject)->GetLocalBounds(Min, Max);

	//Method to move the object to our newly selected position
	GetStaticMesh(CurrentObject)->SetWorldLocation(HitSurface.ImpactPoint + HitSurface.Normal*((-Min) * GetStaticMesh(CurrentObject)->GetComponentScale()));

	//Reset ignored parameters
	TraceParams.ClearIgnoredComponents();


	//Remove the reference to the object because we are not holding it any more
	if (bRightHandSelected)
	{
		RightHandSlot = nullptr;
		/*	DONE via blueprints
		Remove icon of the object in the inventory slot
		*/
		//Reset the positioning vector back to default state
		RightZPos = 30.f;
		RightYPos = 20;

		//Add item in left hand back to ignored actor by line trace
		if (LeftHandSlot)
		{
			TraceParams.AddIgnoredComponent(GetStaticMesh(LeftHandSlot));
		}
	}
	else
	{
		LeftHandSlot = nullptr;
		/*	DONE via blueprints
		Remove icon of the object in the inventory slot
		*/
		//Reset the positioning vector back to default state
		LeftZPos = 30.f;
		LeftYPos = 20;

		//Add item in right hand back to ignored actor by line trace
		if (RightHandSlot)
		{
			TraceParams.AddIgnoredComponent(GetStaticMesh(RightHandSlot));
		}
	}

	//Set collision back to physics body
	//GetStaticMesh(CurrentObject)->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);
	//Reactivate the gravity
	GetStaticMesh(CurrentObject)->SetEnableGravity(true);

	//Remove the reference because we just dropped the item that was selected
	SelectedObject = nullptr;

	//Disable Rotation mode
	bRotationModeAllowed = false;
	//Reset rotation index to default 0
	RotationAxisIndex = 0;
}

void AMyCharacter::SwitchRotationAxis()
{
	//Exit function call if rotation is not permited here
	if (!bRotationModeAllowed)
	{
		return;
	}
	//Increment the index which coresponds to rotation axis
	RotationAxisIndex++;

	//Recycle after the third axis
	if (RotationAxisIndex == 4)
	{
		RotationAxisIndex = 1;
	}
}

void AMyCharacter::RotateObject(const float Value)
{
	//Check if controler is valid, input is not null and that we are in rotation mode
	if ((Controller != nullptr) && (Value != 0.0f) && RotationAxisIndex)
	{
		//Local variable to store the difference gained from the input
		FRotator RotIncrement;

		//Add value to increment based on the axis
		switch (RotationAxisIndex)
		{
		case 1:
			RotIncrement = FRotator(0.f, Value*10.f, 0.f);
			break;
		case 2:
			RotIncrement = FRotator(0.f, 0.f, Value*10.f);
			break;
		case 3:
			RotIncrement = FRotator(Value*10.f, 0.f, 0.f);
			break;
		default:
			return;
		}

		//Add our input to the currently selected object
		if (bRightHandSelected)
		{
			RightHandRotator += RotIncrement;
		}
		else
		{
			LeftHandRotator += RotIncrement;
		}
	}
}

void AMyCharacter::MoveItemZ(const float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		if (bRightHandSelected)
		{
			if ((Value < 0 && RightZPos > 20) || (Value >0 && RightZPos <40))
			{
				RightZPos += Value*0.35f;
			}
		}
		else
		{
			if ((Value < 0 && LeftZPos > 20) || (Value >0 && LeftZPos <40))
			{
				LeftZPos += Value*0.35f;
			}
		}
	}
}

void AMyCharacter::MoveItemY(const float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		if (bRightHandSelected)
		{
			if ((Value < 0 && RightYPos > 5) || (Value >0 && RightYPos <25))
			{
				RightYPos += Value*0.35f;
			}
		}
		else
		{
			if ((Value < 0 && LeftYPos <25) || (Value >0 && LeftYPos >5))
			{
				LeftYPos -= Value*0.35f;
			}
		}
	}
}

void AMyCharacter::UpdateTextBoxes()
{
	//Help text to display at the beginig of the game
	DisplayMessageLeft = TEXT("Use WASD to move around the kitchen.\nYour first goal is to prepare the table \nso that one can eat dinner.");
	DisplayMessageRight = TEXT("Remember that you can use BOTH your hands! \nUse TAB to switch between them!");

	//if (HighlightedActor)
	//{
	//	//Print a message to the screen
	//	//DisplayMessage = TEXT("Press click to interact \n(Grab or Open/Close Action)");
	//}
	//else
	//{
	//	//Reset text to empty
	//	//DisplayMessage = TEXT("Use WASD for movement. \nTAB button to switch between which hand to use.");
	//}

	////DisplayMessage2 = TEXT("");

	////if hand is empty

	//if (!RotationAxisIndex)
	//{
	//	//Display message to tell user that he can rotate an object
	//	//DisplayMessage = TEXT("Press R for rotation mode using the mouse wheel\nClick to drop item");
	//	//DisplayMessage2 = TEXT("You can use the keyboard arrows\nto adjust the position of item in hand");
	//}

	//Text When rotating object
	////Update display text based on Rotation Axis Index
	//switch (RotationAxisIndex)
	//{
	//case 1:
	//	DisplayMessage = TEXT("Rotate about Z axis. R to switch\nClick to drop item");
	//	break;
	//case 2:
	//	DisplayMessage = TEXT("Rotate about Y axis. R to switch\nClick to drop item");
	//	break;
	//case 3:
	//	DisplayMessage = TEXT("Rotate about X axis. R to switch\nClick to drop item");
	//	break;
	//}
}