#include "DaySummaryWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

void UDaySummaryWidget::SetSummaryDisplayTexts(const FDaySummaryDisplayTexts& D)
{
    if (TotalCustomersText)
        TotalCustomersText->SetText(
            FText::FromString("Total Customers: " + D.TotalCustomers.ToString())
        );

    if (SuccessfulPurchasesText)
        SuccessfulPurchasesText->SetText(
            FText::FromString("Successful Purchases: " + D.SuccessfulPurchases.ToString())
        );

    if (FailedPurchasesText)
        FailedPurchasesText->SetText(
            FText::FromString("Failed Purchases: " + D.FailedPurchases.ToString())
        );

    if (FailReasonsText)
        FailReasonsText->SetText(
            FText::FromString("Failed Purchase Reasons:\n" + D.FailReasons.ToString())
        );

    if (AvgExperienceText)
        AvgExperienceText->SetText(
            FText::FromString("Average Satisfaction: " + D.AvgExperience.ToString())
        );

    if (DeltaReputationText)
    {
        DeltaReputationText->SetText(
            FText::FromString("Reputation Change: " + D.DeltaReputation.ToString())
        );
        DeltaReputationText->SetColorAndOpacity(D.DeltaReputationColor);
    }

    if (NewReputationText)
        NewReputationText->SetText(
            FText::FromString("New Reputation: " + D.NewReputation.ToString())
        );

    if (ExpectedCustomersTomorrowText)
        ExpectedCustomersTomorrowText->SetText(
            FText::FromString("Expected Customers Tomorrow: " + D.ExpectedCustomersTomorrow.ToString())
        );

    if (StartingCashText)
        StartingCashText->SetText(
            FText::FromString("Starting Cash: " + D.StartingCash.ToString())
        );

    if (EndingCashText)
        EndingCashText->SetText(
            FText::FromString("Ending Cash: " + D.EndingCash.ToString())
        );

    if (DailyProfitText)
    {
        DailyProfitText->SetText(
            FText::FromString("Daily Profit: " + D.DailyProfit.ToString())
        );
        DailyProfitText->SetColorAndOpacity(D.DailyProfitColor);
    }
}
