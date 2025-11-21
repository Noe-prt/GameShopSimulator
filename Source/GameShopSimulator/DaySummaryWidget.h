#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ClientSimLibrary.h"       // Pour FDaySummaryDisplayTexts
#include "DaySummaryWidget.generated.h"

class UTextBlock;
class UButton;

/**
 * Widget de fin de journée – affichage des stats
 */
UCLASS()
class GAMESHOPSIMULATOR_API UDaySummaryWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    /** Remplit tous les textes du widget avec des données déjà formatées */
    UFUNCTION(BlueprintCallable, Category = "DaySummary")
    void SetSummaryDisplayTexts(const FDaySummaryDisplayTexts& Data);

protected:

    // ----- Text Widgets -----

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UTextBlock* TotalCustomersText;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UTextBlock* SuccessfulPurchasesText;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UTextBlock* FailedPurchasesText;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UTextBlock* FailReasonsText;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UTextBlock* AvgExperienceText;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UTextBlock* DeltaReputationText;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UTextBlock* NewReputationText;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UTextBlock* ExpectedCustomersTomorrowText;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UTextBlock* StartingCashText;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UTextBlock* EndingCashText;

    UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
    UTextBlock* DailyProfitText;
};
