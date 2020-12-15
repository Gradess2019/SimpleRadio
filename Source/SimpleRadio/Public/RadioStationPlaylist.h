// Gradess Games. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RadioStationPlaylist.generated.h"

/**
 * Radio station playlist
 */
UCLASS(BlueprintType, Blueprintable, Config=Radio)
class SIMPLERADIO_API URadioStationPlaylist : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Radio")
	static TArray<FString> GetStreams();
};
