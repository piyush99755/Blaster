#pragma once

UENUM(BlueprintType)
enum class ETurningInPlace : uint8
{
	ETIP_Right UMETA(DisplayName = "Turning Right"),
	ETIP_Left  UMETA(DisplayName = "Turning Left"),
	ETIP_NoTurning UMETA(DisplayName = "No Turning"),

	ETIP_MAX UMETA(DisplayName = "DefaultMAX")
};