// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//Enum used in the TMap which keeps the state of the drawer
UENUM(BlueprintType)
enum class EAssetState : uint8
{
	Closed UMETA(DisplayName = "Closed"),
	Open UMETA(DisplayName = "Open"),
	Unkown UMETA(DisplayName = "Unkown")
};

//Enum used when mapping the items
UENUM(BlueprintType)
enum class EItemType : uint8
{
	GeneralItem UMETA(DisplayName = "GeneralItem"),
	Cup UMETA(DisplayName = "Cup"),
	Plate UMETA(DisplayName = "Plate"),
	Mug	UMETA(DisplayName = "Mug"),
	Pan UMETA(DisplayName = "Pan"),
	Spatula UMETA(DisplayName = "Spatula"),
	Spoon UMETA(DisplayName = "Spoon")
};

#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

UCLASS()
class ROBCOGWEB_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	//Camera component for our character
	class UCameraComponent* MyCharacterCamera;

	//Function to return a string out of the enum type
	template<typename TEnum>
	static FORCEINLINE FString GetEnumValueToString(const FString& Name, TEnum Value)
	{
		const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, *Name, true);
		if (!enumPtr)
		{
			return FString("Invalid");
		}
		return enumPtr->GetEnumName((int32)Value);
	}

	//String used for displaying help messages for the user
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		FString DisplayMessageLeft;

	//String used for displaying help messages for the user
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		FString DisplayMessageRight;

	//Array to store all actors in the world; used to find which object is selected
	TArray<AActor*> AllActors;

	//TMap which keeps the open/closed state for our island drawers
	TMap<AActor*, EAssetState> AssetStateMap;

	//TMap which keeps the interractive items from the kitchen
	TMap<AActor*, EItemType> ItemMap;

	//Actor pointer for the item currently selected
	AActor* SelectedObject;

	//Actor currently focused
	AActor* HighlightedActor;

	//Pointer to the item held in the right hand
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		AActor* RightHandSlot;

	//Pointer to the item held in the left hand
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
		AActor* LeftHandSlot;

	//Parameters for the ray trace
	FCollisionQueryParams TraceParams;

	//Vectors used in ray tracing
	FVector Start;
	FVector End;

	//Line trace Hit Result 
	FHitResult HitObject;

	//Variable for maximum grasping length
	float MaxGraspLength;

	//Variable storing which hand should perform the next action
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	bool bRightHandSelected;

	// Input variable for the force applied on the selected object
	FVector AppliedForce;

	//Vectors used for getting the size of an item
	FVector Min;
	FVector Max;

	//Rotators used for positioning the items held in each hand
	FRotator RightHandRotator;
	FRotator LeftHandRotator;

	//Variables to adjust the position of items held
	float RightZPos;
	float LeftZPos;
	float RightYPos;
	float LeftYPos;

	//Boolean which tells when rotation mode is available
	bool bRotationModeAllowed;
	//Integer to store the index of rotation axis
	int RotationAxisIndex;

	//List to hold all stackables from world
	TSet<AActor*> AllStackableItems;

	//Variable which holds stacked items when manipulated
	TSet<AActor*> TwoHandSlot;


protected:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	/** Handles movement of our character forward/backward */
	void MoveForward(const float Value);

	/** Handles movement of our character, left and right */
	void MoveRight(const float Value);

	//Handles the input from the mouse
	void Click();

	//Method to pick up stack
	void GrabWithTwoHands();

	//Switches between which hand will perform the next action
	void SwitchSelectedHand();

	//Function which returns the static mesh component of the selected object; NOT efficient --> Look for alternatives
	UStaticMeshComponent* GetStaticMesh(AActor* Actor);

	//Function to pick an item in one of our hands
	void PickToInventory(AActor* CurrentObject);

	//Function to release the currently held item
	void DropFromInventory(AActor* CurrentObject, FHitResult HitSurface);

	//Function to open / close drawers and doors
	void OpenCloseAction(AActor* OpenableActor);

	//Function to switch between the rotation axis each time player presses a key
	void SwitchRotationAxis();

	//Function to rotate based on the input from mouse wheel
	void RotateObject(const float Value);

	//Function to move the selected object to the right/left
	void MoveItemY(const float Value);

	//Function to move the selected object up/down
	void MoveItemZ(const float Value);

	//Function called to update the tips which guide the player
	void UpdateTextBoxes();

	//Function to place an item on top of surface or another object in the world
	void PlaceOnTop(AActor* ActorToPlace, FHitResult HitSurface);

	//Method used to get an arranged list of items which are stacked on top of eachother
	TSet<AActor*> GetStack(AActor* ContainedItem);

public:
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetMyCharacterCamera() const { return MyCharacterCamera; }

};