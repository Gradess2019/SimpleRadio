// Gradess Games. All rights reserved.

#include "Actors/Radio.h"
#include "MediaSoundComponent.h"
#include "RadioStationPlaylist.h"
#include "MediaPlayer.h"
#include "Interfaces/RadioReplicator.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

ARadio::ARadio()
{
	MediaSoundComponent = CreateDefaultSubobject<UMediaSoundComponent>(FName("MediaSound"));
	MediaSoundComponent->SetupAttachment(GetRootComponent());
}

void ARadio::BeginPlay()
{
	Super::BeginPlay();
}

void ARadio::SetupStreams_Implementation()
{
	Streams = URadioStationPlaylist::GetStreams();
}

void ARadio::Open_Implementation(const FString& URL)
{
	auto MediaPlayer = GetMedia();
	MediaPlayer->Close();
	MediaPlayer->OpenUrl(URL);
}

UMediaPlayer* ARadio::GetMedia_Implementation()
{
	return MediaSoundComponent->GetMediaPlayer();
}

void ARadio::OnRep_CurrentStream_Implementation()
{
	Open(CurrentStream);
}

void ARadio::Play_Implementation(const FString& URL)
{
	const auto bAuthority = HasAuthority();
	if (bAuthority)
	{
		if (!bLock) { return; }

		CurrentStream = URL;
		Lock();

		#if UE_SERVER
		UE_LOG(LogTemp, Warning, TEXT("Dedicated server"));
		FTimerHandle UnlockTimer;
		GetWorldTimerManager().SetTimer(
			UnlockTimer,
			this,
			&ARadio::Unlock,
			2.f
		);
		#endif
	} else
	{
		const auto PlayerController = UGameplayStatics::GetPlayerController(this, 0);
		const auto bImplements = PlayerController->Implements<URadioReplicator>();
		if (!bImplements) { return; }

		IRadioReplicator::Execute_Play(PlayerController, URL);
	}
}
