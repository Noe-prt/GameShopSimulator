#include "ClientSimLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// -----------------------------------------------------
//   EXPERIENCE CLIENT
// -----------------------------------------------------

float UClientSimLibrary::ComputeClientExperience(const FClientReport& R)
{
	const float MarketPrice = R.MarketPrice;
	const float PaidPrice = R.PaidPrice;

	// ----------------------
	//    A ACHETÉ
	// ----------------------
	if (R.HasBought)
	{
		if (MarketPrice <= 0.0f)
			return 0.0f; // sécurité

		float BaseScore = 0.2f;

		float Diff = MarketPrice - PaidPrice;
		float DiffRatio = Diff / MarketPrice;

		float DiscountBonus = 0.0f;
		if (DiffRatio > 0.0f)
		{
			// plus c'est en dessous du marché, plus c'est cool, max +0.6
			DiscountBonus = FMath::Clamp(DiffRatio * 0.8f, 0.0f, 0.6f);
		}

		float OverPenalty = 0.0f;
		if (DiffRatio < 0.0f)
		{
			float Over = -DiffRatio;
			OverPenalty = FMath::Clamp(Over * 0.5f, 0.0f, 0.4f);
		}

		float Score = BaseScore + DiscountBonus - OverPenalty;
		return FMath::Clamp(Score, -1.0f, 1.0f);
	}

	// ----------------------
	//   N'A PAS ACHETÉ
	// ----------------------
	switch (R.FailReason)
	{
	case EClientFailReason::TooExpensive:
	{
		if (MarketPrice <= 0.0f)
			return -0.3f;

		float Diff = PaidPrice - MarketPrice;
		float OverRatio = Diff / MarketPrice;

		float Penalty = FMath::Clamp(OverRatio * 1.5f, 0.1f, 0.8f);
		return -Penalty;
	}

	case EClientFailReason::NotFound:
		return -0.3f;

	case EClientFailReason::NoCashier:
		return -0.9f;

	case EClientFailReason::NoPrice:
		return -0.6f;

	default:
		return 0.0f;
	}
}

float UClientSimLibrary::ComputeEndOfDayReputationDelta(
	const TArray<FClientReport>& Reports,
	float DailyReputationFactor,
	float& OutAverageExperience)
{
	OutAverageExperience = 0.0f;

	if (Reports.Num() == 0)
		return 0.0f;

	float Sum = 0.0f;

	for (const FClientReport& R : Reports)
	{
		Sum += ComputeClientExperience(R);
	}

	OutAverageExperience = Sum / Reports.Num(); // entre -1 et +1

	const float DeltaRep = OutAverageExperience * DailyReputationFactor;
	return DeltaRep;
}

// -----------------------------------------------------
//   PRICING / ACHAT
// -----------------------------------------------------

float UClientSimLibrary::ComputePerceivedPrice(float Price, float PerceptionThreshold)
{
	// Clamp le seuil dans [0, 0.99] pour éviter les comportements bizarres
	const float ClampedThreshold = FMath::Clamp(PerceptionThreshold, 0.0f, 0.99f);

	if (Price < 0.0f)
	{
		// Cas non prévu normalement, on renvoie tel quel
		return Price;
	}

	const float IntPart = FMath::FloorToFloat(Price);
	const float Fraction = Price - IntPart;

	// Exemple : 4.75 avec threshold 0.75 → perçu comme 4.0
	if (Fraction <= ClampedThreshold)
	{
		return IntPart;
	}
	else
	{
		return IntPart + 1.0f;
	}
}

float UClientSimLibrary::ComputePurchaseProbability(
	float MarketPrice,
	float StorePrice,
	float Popularity,
	float BasePriceSensitivity,
	float PerceptionThreshold)
{
	if (MarketPrice <= 0.0f)
	{
		return 0.0f;
	}

	// 1) Prix perçu
	const float PerceivedPrice = ComputePerceivedPrice(StorePrice, PerceptionThreshold);

	// 2) Ratio prix
	const float PriceRatio = PerceivedPrice / MarketPrice;
	const float OverpriceRatio = PriceRatio - 1.0f;

	// 3) Popularité inversée
	// Popularity = [0..1]
	const float ClampedPopularity = FMath::Clamp(Popularity, 0.0f, 1.0f);

	// NOUVELLE logique :
	// Jeu très populaire (1.0)  -> FAIBLE sensibilité au prix  (0.7x)
	// Jeu peu populaire  (0.0)  -> FORTE sensibilité au prix  (1.3x)

	const float MinMultiplier = 0.7f; // popularité haute
	const float MaxMultiplier = 1.3f; // popularité basse

	// On inverse le lerp :
	const float SensitivityMultiplier =
		FMath::Lerp(MaxMultiplier, MinMultiplier, ClampedPopularity);

	const float PriceSensitivity = BasePriceSensitivity * SensitivityMultiplier;

	// 4) Calcul du facteur
	float PriceFactor = 1.0f;

	if (OverpriceRatio <= 0.0f)
	{
		PriceFactor = 1.0f;
	}
	else
	{
		PriceFactor = 1.0f - (OverpriceRatio * PriceSensitivity);
	}

	return FMath::Clamp(PriceFactor, 0.0f, 1.0f);
}


FDaySummaryDisplayTexts UClientSimLibrary::FormatDaySummaryForUI(const FDaySummaryData& D)
{
	FDaySummaryDisplayTexts Out;

	// ---- Simple ints ----
	Out.TotalCustomers = FText::AsNumber(D.TotalCustomers);
	Out.SuccessfulPurchases = FText::AsNumber(D.SuccessfulPurchases);
	Out.FailedPurchases = FText::AsNumber(D.FailedPurchases);

	// ---- Fail reasons breakdown ----
	FString FR =
		FString::Printf(TEXT("Too Expensive: %d\n"), D.Fail_TooExpensive) +
		FString::Printf(TEXT("Not Found: %d\n"), D.Fail_NotFound) +
		FString::Printf(TEXT("No Cashier: %d\n"), D.Fail_NoCashier) +
		FString::Printf(TEXT("No Price: %d"), D.Fail_NoPrice);

	Out.FailReasons = FText::FromString(FR);

	// ---- Average Experience ----
	Out.AvgExperience = FText::FromString(
		FString::Printf(TEXT("%.2f"), D.AverageExperience)
	);

	// ---- Delta Reputation (with sign and %) ----
	float DeltaPercent = D.DeltaReputation * 100.0f;
	FString DeltaStr = FString::Printf(TEXT("%+.2f%%"), DeltaPercent);
	Out.DeltaReputation = FText::FromString(DeltaStr);

	// color (green if positive, red if negative)
	Out.DeltaReputationColor = (D.DeltaReputation >= 0.0f)
		? FLinearColor(0.1f, 1.0f, 0.1f) // green
		: FLinearColor(1.0f, 0.15f, 0.15f); // red

	// ---- New Reputation (converted to %) ----
	FString NewRepStr = FString::Printf(TEXT("%.0f%%"), D.NewReputation * 100.0f);
	Out.NewReputation = FText::FromString(NewRepStr);

	// ---- Expected customers tomorrow ----
	Out.ExpectedCustomersTomorrow = FText::AsNumber(D.ExpectedCustomersTomorrow);

	// ---- Cash summary ----
	Out.StartingCash = FText::Format(
		FText::FromString(TEXT("${0}")),
		FText::AsNumber(FMath::RoundToInt(D.StartingCash))
	);

	Out.EndingCash = FText::Format(
		FText::FromString(TEXT("${0}")),
		FText::AsNumber(FMath::RoundToInt(D.EndingCash))
	);

	float Profit = D.EndingCash - D.StartingCash;

	Out.DailyProfit = FText::FromString(
		FString::Printf(TEXT("%+d$"), FMath::RoundToInt(Profit))
	);

	Out.DailyProfitColor = (Profit >= 0)
		? FLinearColor(0.1f, 1.0f, 0.1f)
		: FLinearColor(1.0f, 0.2f, 0.2f);

	return Out;
}

int32 UClientSimLibrary::ComputeClientsForNextDay(
	float StoreReputation,
	int32 MinClients,
	int32 MaxClients)
{
	// Clamp la réputation pour sécurité
	float R = FMath::Clamp(StoreReputation, 0.0f, 1.0f);

	// Assure que les bornes sont cohérentes
	if (MaxClients < MinClients)
		Swap(MinClients, MaxClients);

	// Interpolation linéaire
	float ClientFloat = FMath::Lerp(
		static_cast<float>(MinClients),
		static_cast<float>(MaxClients),
		R
	);

	return FMath::RoundToInt(ClientFloat);
}


FDaySummaryData UClientSimLibrary::BuildDaySummaryFromReports(
	const TArray<FClientReport>& Reports,
	float StartingCash,
	float EndingCash,
	float DailyReputationFactor,
	float CurrentReputation,
	int32 MinClients,
	int32 MaxClients)
{
	FDaySummaryData D;

	// -----------------------------
	// COUNT TOTALS
	// -----------------------------
	D.TotalCustomers = Reports.Num();

	D.SuccessfulPurchases = 0;
	D.FailedPurchases = 0;

	D.Fail_TooExpensive = 0;
	D.Fail_NotFound = 0;
	D.Fail_NoCashier = 0;
	D.Fail_NoPrice = 0;

	for (const FClientReport& R : Reports)
	{
		if (R.HasBought)
		{
			D.SuccessfulPurchases++;
		}
		else
		{
			D.FailedPurchases++;

			switch (R.FailReason)
			{
			case EClientFailReason::TooExpensive:
				D.Fail_TooExpensive++;
				break;

			case EClientFailReason::NotFound:
				D.Fail_NotFound++;
				break;

			case EClientFailReason::NoCashier:
				D.Fail_NoCashier++;
				break;

			case EClientFailReason::NoPrice:
				D.Fail_NoPrice++;
				break;

			default:
				break;
			}
		}
	}

	// -----------------------------
	// CALCULATE AVERAGE EXPERIENCE + DELTA REPUTATION
	// -----------------------------
	float AvgExp = 0.0f;
	float DeltaRep = ComputeEndOfDayReputationDelta(
		Reports,
		DailyReputationFactor,
		AvgExp
	);

	D.AverageExperience = AvgExp;
	D.DeltaReputation = DeltaRep;

	// -----------------------------
	// UPDATE REPUTATION
	// -----------------------------
	float NewReputation = FMath::Clamp(CurrentReputation + DeltaRep, 0.0f, 1.0f);
	D.NewReputation = NewReputation;

	// -----------------------------
	// TOMORROW CLIENT FORECAST
	// -----------------------------
	D.ExpectedCustomersTomorrow =
		ComputeClientsForNextDay(NewReputation, MinClients, MaxClients);

	// -----------------------------
	// CASH
	// -----------------------------
	D.StartingCash = StartingCash;
	D.EndingCash = EndingCash;

	return D;
}

FText UClientSimLibrary::GetShopTimeText(
	float DayDurationSeconds,
	float DayElapsedSeconds,
	float StartHour,
	float EndHour)
{
	if (DayDurationSeconds <= 0.0f)
		return FText::FromString(TEXT("00:00"));

	// 1. Ratio du jour (0 à 1)
	float Ratio = FMath::Clamp(DayElapsedSeconds / DayDurationSeconds, 0.0f, 1.0f);

	// 2. Heure virtuelle en décimal
	float CurrentHourFloat = StartHour + Ratio * (EndHour - StartHour);

	// 3. Extraire heures et minutes
	int32 Hour = FMath::FloorToInt(CurrentHourFloat);

	float Fraction = CurrentHourFloat - Hour;
	int32 Minute = FMath::RoundToInt(Fraction * 60.0f);

	// Empêcher 60 mins (arrondi) → passer à l'heure suivante
	if (Minute >= 60)
	{
		Minute = 0;
		Hour++;
	}

	// 4. Format "HH:MM" avec zéros devant
	FString HourStr = FString::Printf(TEXT("%02d"), Hour);
	FString MinuteStr = FString::Printf(TEXT("%02d"), Minute);

	return FText::FromString(HourStr + TEXT(":") + MinuteStr);
}



