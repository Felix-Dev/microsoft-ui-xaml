// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.


#include "HexadecimalFormatter.g.h"
#include "HexadecimalFormatter.properties.h"

#pragma once
class HexadecimalFormatter :
    public winrt::implementation::HexadecimalFormatterT<HexadecimalFormatter>,
    public HexadecimalFormatterProperties
{
public:
    HexadecimalFormatter();

#pragma region INumberFormatter2
    winrt::hstring FormatDouble(double value);
    winrt::hstring FormatInt(__int64 value);
    winrt::hstring FormatUInt(unsigned __int64 value);
#pragma endregion

#pragma region INumberParser
    winrt::IReference<double> ParseDouble(winrt::hstring text);
    winrt::IReference<__int64> ParseInt(winrt::hstring text);
    winrt::IReference<unsigned __int64> ParseUInt(winrt::hstring text);
#pragma endregion

private:
    winrt::hstring NumberToString(__int64 value, int numDigits);
    wchar_t NormalizeCharDigit(wchar_t c);

    int GetStartIndex(__int64 value);
    winrt::hstring GetStringPrefix(const winrt::hstring& string, int numDigits);

    wchar_t* TrimLeadingZeroes(wchar_t* text, int* textLength);
};

