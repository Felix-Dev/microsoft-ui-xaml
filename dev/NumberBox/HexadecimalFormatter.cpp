// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#include <pch.h>
#include <common.h>
#include <Vector.h>
#include "HexadecimalFormatter.h"

// digits 0..15 used by the hexadecimal numeral system
static constexpr wstring_view HEX_DIGITS = L"0123456789ABCDEF";

HexadecimalFormatter::HexadecimalFormatter()
{
    const auto inputPrefixes = winrt::make<Vector<winrt::IInspectable, MakeVectorParam<VectorFlag::Observable>()>>();
    SetValue(s_InputPrefixesProperty, inputPrefixes);
}

// INumberFormatter2

winrt::hstring HexadecimalFormatter::FormatDouble(double value)
{
    // We are copying WinForm's NumericUpDown control behavior here which truncates a double value like this: X.YZ -> X
    const auto truncatedValue = static_cast<__int64>(value);

    // "trim" leading zeroes by computing the index of the first non-zero byte in the byte representation
    // of the specified value
    const int numEssentialDigits = GetStartIndex(truncatedValue);

    // convert the given value to a correspnding string representation 
    const auto numberResult = NumberToString(truncatedValue, numEssentialDigits);

    // get the output prefix to add to the string representation
    const auto outputPrefix = GetStringPrefix(numberResult, numEssentialDigits);

    // append the rcomputed output prefix to the string representation and return the result
    const auto endResult = static_cast<std::wstring>(outputPrefix).append(numberResult);
    return winrt::hstring(endResult);
}

int HexadecimalFormatter::GetStartIndex(__int64 value)
{
    // We have a datatype 64 bits wide so we can display 16 hex digits with it
    int digits = 16;

    int i = 0;
    while ((value >> (60 - i)) == 0)
    {
        digits--;

        // If there is only one digit left, we are done here as we will process that digit,
        // no matter its value.
        if (digits == 1)
        {
            break;
        }

        // Increase our shift amount to get to the next digit.
        i += 4;
    }

    return digits;
}

winrt::hstring HexadecimalFormatter::GetStringPrefix(const winrt::hstring& string, int numDigits)
{
    auto minDigits = MinDigits();
    if (minDigits < 1)
    {
        minDigits = 1;
    }
    else if (minDigits > 16)
    {
        minDigits = 16;
    }

    const auto stringPrefix = OutputPrefix();
    if (minDigits > numDigits)
    {
        // The requested number of digits is more than the actual number of digits needed to represent
        // the specified value. As such, we will add additional leading "0"s to it until we matched the requested amount.
        const int digitsToAdd = minDigits - numDigits;
        std::wstring digitPrefix(digitsToAdd, '0');

        return winrt::hstring((static_cast<std::wstring>(stringPrefix)).append(digitPrefix));
    }

    return stringPrefix;
}

winrt::hstring HexadecimalFormatter::NumberToString(__int64 value, int numDigits)
{
    std::wstring result = L"";

    // "numDigits" tells us how many actual digits are needed to correctly represent the given value.
    // Here, we will start from the the highest required digits and "work our way down" the essential digits,
    // converting each digit to its string representation along the way while appending them.
    const bool isGrouped = IsGrouped();
    while (numDigits-- > 0)
    {
        const int curHexValue = (value >> (4 * numDigits)) & 0xF;
        result += HEX_DIGITS[curHexValue];

        // If the output should be grouped (every 4 digits starting from right are considered a group),
        // we add a new whitespace between every two digit groups. numDigits contains the number of digits
        // to process still left so if its a multiple of four, we know that we have to insert a space as the
        // next character to be processed is the final charatcer of a group.
        //
        // TODO: Possible improvement: isGrouped is a constant here in the loop so perhaps we can create two versions of the loop here in order
        // to only check isGrouped once.
        if (isGrouped
            && (numDigits % 4) == 0 && numDigits > 0)
        {
            result += ' ';
        }
    }

    return winrt::hstring(result);
}

winrt::hstring HexadecimalFormatter::FormatInt(__int64 value)
{
    winrt::throw_hresult(E_NOTIMPL);
}

winrt::hstring HexadecimalFormatter::FormatUInt(unsigned __int64 value)
{
    winrt::throw_hresult(E_NOTIMPL);
}

// INumberParser

const std::wstring c_whitespace = L" \n\r\t\f\v";

wchar_t* HexadecimalFormatter::TrimLeadingZeroesAndSpaces(wchar_t* text, int* textLength)
{
    int sTextLength = *textLength;
    int i;
    for (i = 0; i < sTextLength - 1; i++)
    {
        const auto curChar = text[i];
        if (curChar == '0' || c_whitespace.find(curChar) != wstring_view::npos)
        {
            continue;
        }

        break;
    }

    *textLength = sTextLength - i;
    return (text + i);
}

winrt::IReference<double> HexadecimalFormatter::ParseDouble(winrt::hstring text)
{
    std::wstring wText = std::wstring(text);
    wchar_t* pText = wText.data();

    int textLength = static_cast<int32_t>(wText.length());

    // First step: Handle custom input prefixes and ignore them in the next input processing steps

    for (const auto inputPrefix : InputPrefixes())
    {
        // TODO: We have to do this if-check for now because InputPrefixes is an IList<object> currently.
        // It should really be an IList<string> but the generated code does not compile.
        // I mentioned this issue in the WinUI repo.
        if (const auto rInputPrefixString = inputPrefix.try_as<winrt::IReference<winrt::hstring>>())
        {
            const auto inputPrefixString = rInputPrefixString.Value();
            if (wText._Starts_with(inputPrefixString))
            {
                const auto length = inputPrefixString.size();
                pText += length;
                textLength -= length;

                break;
            }
        }
    }

    // Second step: Trim leading zeroes and spaces

    pText = TrimLeadingZeroesAndSpaces(pText, &textLength);

    // Third step: Convert from string form to matching number form, like L"80000000" -> 0x80000000
    // Here, we iterate over each character and try to convert each character to its corresponding
    // numerical value and shifting it into the correct position.

    uint64_t w64Bits = 0;
    for (int i = 0; i < textLength; i++)
    {
        auto curChar = pText[i];

        // If the current character is a whitespace character we skip it (we thus allow spaces between digits).
        // TODO: COuld we move such a function into a string utility class (i.e. bool StringUtils::IsWhitespaceCharacter())?
        if (c_whitespace.find(curChar) != wstring_view::npos)
        {
            continue;
        }

        // We allow both upper and lower case letters for "A" - "F".
        curChar = NormalizeCharDigit(curChar);

        const size_t pos = HEX_DIGITS.find(curChar);
        if (pos != wstring_view::npos)
        {
            // We could successfully convert the current character into its matching numerical
            // representation.
            // As in any number base the value of the i-th digit is d * base^i (where d is the digit, b is the base and i is the index of the digit in the number),
            // the actual decimal value of the number is the sum of (i = 0 - [num bits -1]) -> d * base^i.
            // In our case here, our base is 16 and we follow the formula above to compute the actual decimal value of the given string number.
            // Since we start with the upper digits first, each character processed here will be multipled with the value 16 (our base) one more time than
            // its direct successor in the string. In other words, by the time we are done, the first character here will have been multiplied with 16
            // by (textLength - 1) times, whereas the last character processed here as been multipled with 16 zero times.
            // (All characters in between, starting from the end of the string, will have been multipled with 16 one time more than their neighbor
            // digit to their immediate right, so the second-last processed digit was multipled with 16 <once>, the third-last processed character <twice>,...).
            //
            // This matches the exact formula specified above as [the number of times a digit is multipled by 16] here is its [index i] in the string.
            w64Bits = w64Bits << 4; // value *= 16;
            w64Bits += static_cast<int32_t>(pos);
        }
        else
        {
            // The current character we attempted to convert into its equivalent numerical representation was not a valid
            // hexadecimal character. Abort conversion process now.
            return nullptr;
        }
    }

    // Final step: Find out if we have a negative number and if we do, handle it accordingly
    double value = static_cast<double>(w64Bits);
    const bool sign = (w64Bits >> 63) & 1;
    if (sign)
    {
        // If the sign bit is set, we get the decimal number in negative 2's complement form.
        value = -static_cast<double>(((w64Bits ^ 0xFFFFFFFFFFFFFFFF) + 1));
    }

    return value;
}

wchar_t HexadecimalFormatter::NormalizeCharDigit(wchar_t c)
{
    return static_cast<wchar_t>(toupper(c));
}

winrt::IReference<__int64> HexadecimalFormatter::ParseInt(winrt::hstring text)
{
    winrt::throw_hresult(E_NOTIMPL);
}

winrt::IReference<unsigned __int64> HexadecimalFormatter::ParseUInt(winrt::hstring text)
{
    winrt::throw_hresult(E_NOTIMPL);
}
