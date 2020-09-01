using Microsoft.UI.Xaml.Controls;
using System;
using System.Collections.Generic;
using System.Linq;
using Windows.Globalization.NumberFormatting;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace MUXControlsTestApp
{
    public sealed partial class NumberBoxNumberFormatterPage : TestPage
    {
        private string currentInputPrefixString = "";

        private INumberFormatter2 defaultNumberFormatter;

        public NumberBoxNumberFormatterPage()
        {
            this.InitializeComponent();

            this.Loaded += (s, e) =>
            {
                this.defaultNumberFormatter = this.NumberFormatterTestNumberBox.NumberFormatter;

                this.NumeralSystemModeComboBox.SelectionChanged += NumeralSystemModeComboBox_SelectionChanged;
                this.NumeralSystemModeComboBox.SelectedIndex = 0;             
            };
        }

        private void IsGroupedCheckBox_Checked(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            if (this.NumberFormatterTestNumberBox.NumberFormatter is HexadecimalFormatter hexFormatter)
            {
                var newHexFormatter = CreateHexFormatterCopy(hexFormatter);
                newHexFormatter.IsGrouped = true;

                this.NumberFormatterTestNumberBox.NumberFormatter = newHexFormatter;
            }
        }

        private void IsGroupedCheckBox_Unchecked(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            if (this.NumberFormatterTestNumberBox.NumberFormatter is HexadecimalFormatter hexFormatter)
            {
                var newHexFormatter = CreateHexFormatterCopy(hexFormatter);
                newHexFormatter.IsGrouped = false;

                this.NumberFormatterTestNumberBox.NumberFormatter = newHexFormatter;
            }
        }

        private void MinDigitsNumberBox_ValueChanged(NumberBox sender, NumberBoxValueChangedEventArgs args)
        {
            if (this.NumberFormatterTestNumberBox.NumberFormatter is HexadecimalFormatter hexFormatter)
            {
                var newHexFormatter = CreateHexFormatterCopy(hexFormatter);
                newHexFormatter.MinDigits = (int)args.NewValue;

                this.NumberFormatterTestNumberBox.NumberFormatter = newHexFormatter;
            }
        }

        private void InputPrefixesTextBox_LostFocus(object sender, RoutedEventArgs e)
        {
            if (this.NumberFormatterTestNumberBox.NumberFormatter is HexadecimalFormatter hexFormatter)
            {
                if (this.currentInputPrefixString != this.InputPrefixesTextBox.Text)
                {
                    this.currentInputPrefixString = this.InputPrefixesTextBox.Text;
                    UpdateInputPrefixes(hexFormatter);
                }
            }
        }

        private void OutputPrefixTextBox_LostFocus(object sender, RoutedEventArgs e)
        {
            if (this.NumberFormatterTestNumberBox.NumberFormatter is HexadecimalFormatter hexFormatter)
            {
                if (hexFormatter.OutputPrefix != this.OutputPrefixTextBox.Text)
                {
                    var newHexFormatter = CreateHexFormatterCopy(hexFormatter);
                    newHexFormatter.OutputPrefix = this.OutputPrefixTextBox.Text;

                    // Manually add the new output prefix to the list of input prefixes
                    if (newHexFormatter.OutputPrefix != "" 
                        && !newHexFormatter.InputPrefixes.Contains(this.OutputPrefixTextBox.Text))
                    {
                        newHexFormatter.InputPrefixes.Add(this.OutputPrefixTextBox.Text);
                        UpdateInputPrefixesTextBoxText(newHexFormatter);
                    }

                    this.NumberFormatterTestNumberBox.NumberFormatter = newHexFormatter;
                }
            }
        }

        private void NumeralSystemModeComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (this.NumeralSystemModeComboBox.SelectedIndex == 0 /* Decimal Mode*/)
            {
                this.NumberFormatterTestNumberBox.NumberFormatter = defaultNumberFormatter;

                this.InputPrefixesTextBox.IsEnabled = false;
                this.OutputPrefixTextBox.IsEnabled = false;
                this.IsGroupedCheckBox.IsEnabled = false;
                this.MinDigitsNumberBox.IsEnabled = false;
            }
            else if (this.NumeralSystemModeComboBox.SelectedIndex == 1 /* Hexadecimal Mode*/)
            {
                this.InputPrefixesTextBox.IsEnabled = true;
                this.OutputPrefixTextBox.IsEnabled = true;
                this.IsGroupedCheckBox.IsEnabled = true;
                this.MinDigitsNumberBox.IsEnabled = true;

                var hexFormatter = new HexadecimalFormatter() {
                    IsGrouped = (bool)this.IsGroupedCheckBox.IsChecked,
                    MinDigits = (int)this.MinDigitsNumberBox.Value,
                    OutputPrefix = this.OutputPrefixTextBox.Text,
                };

                UpdateInputPrefixes(hexFormatter);

                this.NumberFormatterTestNumberBox.NumberFormatter = hexFormatter;
            }
        }

        private void UpdateInputPrefixes(HexadecimalFormatter hexFormatter)
        {
            var iInputPrefixes = this.InputPrefixesTextBox.Text
                    .Split(new char[] { ',' }, StringSplitOptions.RemoveEmptyEntries);

            var newHexFormatter = CreateHexFormatterCopy(hexFormatter);
            newHexFormatter.InputPrefixes.Clear();

            foreach (var inputPrefix in iInputPrefixes)
            {
                newHexFormatter.InputPrefixes.Add(inputPrefix.Trim());
            }

            this.NumberFormatterTestNumberBox.NumberFormatter = newHexFormatter;
        }

        private void UpdateInputPrefixesTextBoxText(HexadecimalFormatter hexadecimalFormatter)
        {
            this.InputPrefixesTextBox.Text = "";

            int i;
            for (i = 0; i < hexadecimalFormatter.InputPrefixes.Count - 1; i++)
            {
                this.InputPrefixesTextBox.Text += $"{hexadecimalFormatter.InputPrefixes[i]}, ";
            }

            this.InputPrefixesTextBox.Text += $"{hexadecimalFormatter.InputPrefixes[i]}";

            this.currentInputPrefixString = this.InputPrefixesTextBox.Text;
        }

        private HexadecimalFormatter CreateHexFormatterCopy(HexadecimalFormatter hexFormatter)
        {
            var newHexFormatter = new HexadecimalFormatter() {
                IsGrouped = hexFormatter.IsGrouped,
                MinDigits = hexFormatter.MinDigits,
                OutputPrefix = hexFormatter.OutputPrefix,
            };

            foreach (var inputPrefix in hexFormatter.InputPrefixes)
            {
                newHexFormatter.InputPrefixes.Add(inputPrefix);
            }

            return newHexFormatter;
        }
    }
}
