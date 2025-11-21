// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ClientSimLibrary.generated.h"

// ---------------- ENUM ----------------

UENUM(BlueprintType)
enum class EClientFailReason : uint8
{
	None        UMETA(DisplayName = "None"),
	TooExpensive UMETA(DisplayName = "Too Expensive"),
	NotFound    UMETA(DisplayName = "Not Found"),
	NoCashier   UMETA(DisplayName = "No Cashier"),
	NoPrice     UMETA(DisplayName = "No Price")
};

// ---------------- CLIENT REPORT STRUCT ----------------

USTRUCT(BlueprintType)
struct FClientReport
{
	GENERATED_BODY()

	// Le client a-t-il acheté ?
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HasBought = false;

	// Raison de l’échec si pas d’achat
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EClientFailReason FailReason = EClientFailReason::None;

	// Prix du marché du jeu ciblé
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MarketPrice = 0.0f;

	// Prix payé dans TON magasin (ou affiché en rayon)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PaidPrice = 0.0f;
};

// ---------------- FUNCTION LIBRARY ----------------

// ---------------- DAY SUMMARY DATA (input) ----------------

USTRUCT(BlueprintType)
struct FDaySummaryData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TotalCustomers = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 SuccessfulPurchases = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 FailedPurchases = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Fail_TooExpensive = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Fail_NotFound = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Fail_NoCashier = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Fail_NoPrice = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AverageExperience = 0.0f; // [-1..1]

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DeltaReputation = 0.0f;    // [-1..1] * DailyFactor

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float NewReputation = 0.0f;      // [0..1]

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ExpectedCustomersTomorrow = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StartingCash = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EndingCash = 0.0f;
};


// ---------------- DAY SUMMARY DISPLAY DATA (output) ----------------

USTRUCT(BlueprintType)
struct FDaySummaryDisplayTexts
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FText TotalCustomers;

	UPROPERTY(BlueprintReadWrite)
	FText SuccessfulPurchases;

	UPROPERTY(BlueprintReadWrite)
	FText FailedPurchases;

	UPROPERTY(BlueprintReadWrite)
	FText FailReasons;

	UPROPERTY(BlueprintReadWrite)
	FText AvgExperience;

	UPROPERTY(BlueprintReadWrite)
	FText DeltaReputation;

	UPROPERTY(BlueprintReadWrite)
	FLinearColor DeltaReputationColor;

	UPROPERTY(BlueprintReadWrite)
	FText NewReputation;

	UPROPERTY(BlueprintReadWrite)
	FText ExpectedCustomersTomorrow;

	UPROPERTY(BlueprintReadWrite)
	FText StartingCash;

	UPROPERTY(BlueprintReadWrite)
	FText EndingCash;

	UPROPERTY(BlueprintReadWrite)
	FText DailyProfit;

	UPROPERTY(BlueprintReadWrite)
	FLinearColor DailyProfitColor;
};


// ---------------- UI FORMATTER FUNCTION ----------------

UCLASS()
class GAMESHOPSIMULATOR_API UClientSimLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/** Calcule l'expérience client (-1 à +1) à partir d'un rapport */
	UFUNCTION(BlueprintPure, Category = "Client|Reputation")
	static float ComputeClientExperience(const FClientReport& Report);

	/**
	 * Calcule :
	 * - la moyenne des expériences du jour (OutAverageExperience, dans [-1..1])
	 * - et renvoie le delta de réputation à appliquer au magasin
	 */
	UFUNCTION(BlueprintPure, Category = "Client|Reputation")
	static float ComputeEndOfDayReputationDelta(
		const TArray<FClientReport>& Reports,
		float DailyReputationFactor,
		float& OutAverageExperience
	);

	UFUNCTION(BlueprintPure, Category = "Client|Flow")
	static int32 ComputeClientsForNextDay(
		float StoreReputation,
		int32 MinClients,
		int32 MaxClients
	);


	/**
	 * Calcule le prix "perçu" par le client.
	 * Exemple : Price=4.75, Threshold=0.75 → 4.0 (le client voit encore "4")
	 */
	UFUNCTION(BlueprintPure, Category = "Client|Pricing")
	static float ComputePerceivedPrice(float Price, float PerceptionThreshold);

	/**
	 * Calcule la probabilité d'achat d'un client pour un jeu donné.
	 *
	 * MarketPrice          : prix du marché pour ce jeu
	 * StorePrice           : prix dans TON magasin
	 * Popularity           : popularité du jeu [0..1]
	 * BasePriceSensitivity : sensibilité de base au prix (ex: 1.5)
	 * PerceptionThreshold  : seuil psychologique (ex: 0.75, 0.99...)
	 *
	 * Retourne une valeur entre 0 et 1.
	 */
	UFUNCTION(BlueprintPure, Category = "Client|Pricing")
	static float ComputePurchaseProbability(
		float MarketPrice,
		float StorePrice,
		float Popularity,
		float BasePriceSensitivity,
		float PerceptionThreshold
	);

	UFUNCTION(BlueprintPure, Category = "UI|Summary")
	static FDaySummaryData BuildDaySummaryFromReports(
		const TArray<FClientReport>& Reports,
		float StartingCash,
		float EndingCash,
		float DailyReputationFactor,
		float CurrentReputation,
		int32 MinClients,
		int32 MaxClients
	);

	UFUNCTION(BlueprintPure, Category = "Time|DayCycle")
	static FText GetShopTimeText(
		float DayDurationSeconds,
		float DayElapsedSeconds,
		float StartHour,
		float EndHour
	);

	UFUNCTION(BlueprintPure, Category = "UI|Summary")
	static FDaySummaryDisplayTexts FormatDaySummaryForUI(const FDaySummaryData& Data);
};
