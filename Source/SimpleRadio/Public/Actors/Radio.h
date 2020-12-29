// Gradess Games. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Radio.generated.h"

class UMediaSoundComponent;
class UMediaPlayer;

UCLASS()
class SIMPLERADIO_API ARadio : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	ARadio();

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Radio")
	UMediaSoundComponent* MediaSoundComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Radio")
	bool bAutoPlay;

	UPROPERTY(BlueprintReadWrite, Category = "Radio | Stream")
	bool bLock;

	UPROPERTY(BlueprintReadWrite, ReplicatedUsing=OnRep_CurrentStream, Category = "Radio | Stream")
	FString CurrentStream;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Radio | Stream")
	TArray<FString> Streams;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Radio | Volume")
	float Volume;

	UPROPERTY(BlueprintReadWrite, Category = "Radio | Volume")
	float MinVolume;

	UPROPERTY(BlueprintReadWrite, Category = "Radio | Volume")
	float MaxVolume;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly, Category = "Radio | Stream")
	void SetupStreams();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Radio | Stream")
	void Open(const FString& URL);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Radio | Stream")
	UMediaPlayer* GetMedia();

	UFUNCTION(BlueprintNativeEvent, Category = "Radio | Stream")
	void OnRep_CurrentStream();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Radio | Stream")
	void Play(const FString& URL);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Radio | Stream")
	void PlayById(const int Id);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Radio | Stream")
	void PlayNext();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Radio | Stream")
	void PlayPrevious();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly, Category = "Radio | Stream")
	void Lock();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, BlueprintAuthorityOnly, Category = "Radio | Stream")
	void Unlock();
};
