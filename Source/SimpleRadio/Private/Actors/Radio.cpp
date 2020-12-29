// Gradess Games. All rights reserved.

#include "Actors/Radio.h"
#include "MediaSoundComponent.h"
#include "RadioStationPlaylist.h"
#include "MediaPlayer.h"
#include "Interfaces/RadioReplicator.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Libraries/RadioHelper.h"
#include "Net/UnrealNetwork.h"

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
	}
	else
	{
		const auto RadioReplicator = GetRadioReplicator();
		if (!IsValid(RadioReplicator)) { return; }

		IRadioReplicator::Execute_Play(RadioReplicator, URL);
	}
}

void ARadio::Lock_Implementation()
{
	bLock = true;
}

void ARadio::Unlock_Implementation()
{
	bLock = false;
}

UObject* ARadio::GetRadioReplicator_Implementation()
{
	const auto PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	const auto bImplements = PlayerController->Implements<URadioReplicator>();
	return bImplements ? PlayerController : nullptr;
}

void ARadio::PlayNext_Implementation()
{
	const auto CurrentStreamId = Streams.Find(CurrentStream);
	const auto NextStreamId = (CurrentStreamId + 1) % Streams.Num();

	PlayById(NextStreamId);
}

void ARadio::PlayById_Implementation(const int Id)
{
	const auto bValidIndex = Streams.IsValidIndex(Id);
	if (!bValidIndex) { return; }

	const auto NewStream = Streams[Id];
	Play(NewStream);
}

void ARadio::PlayPrevious_Implementation()
{
	const auto CurrentStreamId = Streams.Find(CurrentStream);
	const auto PreviousStreamId = CurrentStreamId - 1 < 0 ? Streams.Num() - 1 : CurrentStreamId - 1;

	PlayById(PreviousStreamId);
}

void ARadio::AdjustVolume_Implementation(const float Delta)
{
	const auto bServer = HasAuthority();
	if (bServer)
	{
		Volume = CalculateNewVolume(Delta);
	} else
	{
		const auto RadioReplicator = GetRadioReplicator();
		if (!IsValid(RadioReplicator)) { return; }

		IRadioReplicator::Execute_AdjustVolume(RadioReplicator, Delta);
	}
}

float ARadio::CalculateNewVolume_Implementation(const float Delta)
{
	const auto CurrentVolume = URadioHelper::GetMediaVolume(MediaSoundComponent);
	const auto NewVolume = FMath::Clamp(CurrentVolume + Delta, MinVolume, MaxVolume);
	return NewVolume;
}

void ARadio::OnRep_Volume_Implementation()
{
	SetVolume(Volume);
}

void ARadio::SetVolume_Implementation(const float NewVolume)
{
	URadioHelper::SetMediaVolume(MediaSoundComponent, NewVolume);
}

void ARadio::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARadio, CurrentStream);
	DOREPLIFETIME(ARadio, Volume);
	DOREPLIFETIME_CONDITION(ARadio, Streams, COND_InitialOnly);
}
