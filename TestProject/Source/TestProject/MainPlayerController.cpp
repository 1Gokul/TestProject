// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"

#include "InventoryComponent.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

bool AMainPlayerController::IsAnyMenuVisible()
{
	return (bPauseMenuVisible || bInventoryMenuVisible);
}

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bPauseMenuVisible = false;
	bInventoryMenuVisible = false;
	bItemInteractPromptVisible = false;
	bHUDVisible = true;

	// Create the HUD Widget and let it be hidden (It gets displayed once the map loads)
	if (HUDOverlayAsset)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset);

		if (HUDOverlay)
		{
			HUDOverlay->AddToViewport();
			HUDOverlay->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (WEnemyHealthBar)
	{
		EnemyHealthBar = CreateWidget<UUserWidget>(this, WEnemyHealthBar);

		if (EnemyHealthBar)
		{
			EnemyHealthBar->AddToViewport();
			EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
		}

		FVector2D Alignment(0.0f, 0.0f);
		EnemyHealthBar->SetAlignmentInViewport(Alignment);
	}

	if (WPauseMenu)
	{
		PauseMenu = CreateWidget<UUserWidget>(this, WPauseMenu);

		if (PauseMenu)
		{
			PauseMenu->AddToViewport();
			PauseMenu->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void AMainPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/** Update position of Enemy health bar depending on the Enemy's current position */
	if (EnemyHealthBar)
	{
		FVector2D PositionInViewport;

		ProjectWorldLocationToScreen(EnemyLocation, PositionInViewport);

		PositionInViewport.Y -= 90.0f;


		const FVector2D SizeInViewport(300.0f, 25.0f);

		EnemyHealthBar->SetPositionInViewport(PositionInViewport);
		EnemyHealthBar->SetDesiredSizeInViewport(SizeInViewport);
	}
}

void AMainPlayerController::GameModeOnly()
{
	FInputModeGameOnly InputModeGameOnly;

	SetInputMode(InputModeGameOnly);
}

void AMainPlayerController::GameAndUIMode()
{
	FInputModeGameAndUI InputModeGameAndUI;

	SetInputMode(InputModeGameAndUI);
}

bool AMainPlayerController::bUIWidgetCurrentlyActive()
{
	return (bInventoryMenuVisible || bPauseMenuVisible);
}

void AMainPlayerController::DisplayHUD_Implementation()
{
	bHUDVisible = true;
}


void AMainPlayerController::HideHUD_Implementation()
{
	bHUDVisible = false;
}


void AMainPlayerController::DisplayEnemyHealthBar()
{
	if (EnemyHealthBar)
	{
		bEnemyHealthBarVisible = true;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainPlayerController::RemoveEnemyHealthBar()
{
	if (EnemyHealthBar)
	{
		bEnemyHealthBarVisible = false;
		EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AMainPlayerController::TogglePauseMenu()
{
	if (bPauseMenuVisible)
	{
		RemovePauseMenu();
	}
	else
	{
		DisplayPauseMenu();
	}
}

void AMainPlayerController::DisplayItemInteractPrompt_Implementation(const FString& ItemName, float Weight, int32 Value,
                                                                     FName InteractText)
{
	bItemInteractPromptVisible = true;
}

void AMainPlayerController::RemoveItemInteractPrompt_Implementation()
{
	bItemInteractPromptVisible = false;
}

void AMainPlayerController::DisplaySearchPrompt_Implementation(const FString& ItemName, FName SearchText)
{
	bSearchPromptVisible = true;
}

void AMainPlayerController::RemoveSearchPrompt_Implementation()
{
	bSearchPromptVisible = false;
}

void AMainPlayerController::ToggleInventoryMenu(UInventoryComponent* InventoryComponent)
{
	if (bInventoryMenuVisible)
	{
		RemoveInventoryMenu();
	}
	else
	{
		DisplayInventoryMenu(InventoryComponent);
	}
}

void AMainPlayerController::DisplayPauseMenu_Implementation()
{
	// Hide the HUD
	if (bHUDVisible)
	{
		HUDOverlay->SetVisibility(ESlateVisibility::Hidden);
		bHUDVisible = false;
	}

	if (PauseMenu)
	{
		bPauseMenuVisible = true;

		UGameplayStatics::SetGamePaused(GetWorld(), true);

		PauseMenu->SetVisibility(ESlateVisibility::Visible);

		GameAndUIMode();

		bShowMouseCursor = true;
	}
}

void AMainPlayerController::RemovePauseMenu_Implementation()
{
	// Show the HUD
	if (!bHUDVisible)
	{
		HUDOverlay->SetVisibility(ESlateVisibility::Visible);
		bHUDVisible = true;
	}

	if (PauseMenu)
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false);

		//Visibility is set in blueprint

		GameModeOnly();

		bShowMouseCursor = false;

		bPauseMenuVisible = false;
	}
}

void AMainPlayerController::DisplayInventoryMenu_Implementation(UInventoryComponent* InventoryComponent)
{
	// Hide the HUD
	if (bHUDVisible)
	{
		HUDOverlay->SetVisibility(ESlateVisibility::Hidden);
		bHUDVisible = false;
	}

	InventoryComponent->PrepareInventory();

	bInventoryMenuVisible = true;

	UGameplayStatics::SetGamePaused(GetWorld(), true);

	//InventoryMenu->SetVisibility(ESlateVisibility::Visible);

	GameAndUIMode();

	bShowMouseCursor = true;
}


void AMainPlayerController::RemoveInventoryMenu_Implementation()
{
	// Show the HUD
	if (!bHUDVisible)
	{
		HUDOverlay->SetVisibility(ESlateVisibility::Visible);
		bHUDVisible = true;
	}

	bInventoryMenuVisible = false;

	UGameplayStatics::SetGamePaused(GetWorld(), false);

	GameModeOnly();

	bShowMouseCursor = false;
}
