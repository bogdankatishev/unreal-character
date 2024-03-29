// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseMenu.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "SaveGameSubsystem.h"

void UPauseMenu::NativeConstruct()
{

	ResumeButton->OnPressed.AddDynamic(this,&UPauseMenu::OnResumeClick);
	SaveGameButton->OnPressed.AddDynamic(this,&UPauseMenu::OnSaveGameClick);
	QuitButton->OnPressed.AddDynamic(this,&UPauseMenu::OnQuitClick);
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(UGameplayStatics::GetPlayerController(GetWorld(),0),this);
}

void UPauseMenu::OnResumeClick()
{
	UGameplayStatics::SetGamePaused(GetWorld(),false);
	RemoveFromParent();
	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(UGameplayStatics::GetPlayerController(GetWorld(),0));
}

void UPauseMenu::OnSaveGameClick()
{
	USaveGameSubsystem* SG = GetGameInstance()->GetSubsystem<USaveGameSubsystem>();
	SG->WriteSaveGame();

	UGameplayStatics::SetGamePaused(GetWorld(),false);
	RemoveFromParent();
	GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(UGameplayStatics::GetPlayerController(GetWorld(),0));
}

void UPauseMenu::OnQuitClick()
{
	UWorld* World = GetWorld();
	UKismetSystemLibrary::ExecuteConsoleCommand(World,TEXT("quit"));
}