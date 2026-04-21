// Fill out your copyright notice in the Description page of Project Settings.


#include "BindableData.h"

void UBindableData::BindDisplay(UObject* display)
{
	if (!IsValid(display))
	{
		UE_LOG(LogTemp, Warning, TEXT("Trying to bind invalid display"));
		return;
	}

	if (!display->GetClass()->ImplementsInterface(UDisplayDataWidget::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Trying to bind display that does not implement IDisplayDataWidget"));
		return;
	}

	if(Data.IsValid())
		IDisplayDataWidget::Execute_ShowData(display, Data.Get());
	else
		IDisplayDataWidget::Execute_ClearData(display);

	Bindings.Add(display);
}

void UBindableData::DataChanged()
{

	if (Data.IsValid())
	{
		auto data = Data.Get();
		for (auto& display : Bindings)
		{
			if (display.IsValid())
				IDisplayDataWidget::Execute_ShowData(display.Get(), data);
		}
	}
	else
	{
		for (auto& display : Bindings)
		{
			if (display.IsValid())
				IDisplayDataWidget::Execute_ClearData(display.Get());
		}
	}
}