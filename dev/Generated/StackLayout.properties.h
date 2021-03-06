// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

// DO NOT EDIT! This file was generated by CustomTasks.DependencyPropertyCodeGen
#pragma once

class StackLayoutProperties
{
public:
    StackLayoutProperties();

    void DisableVirtualization(bool value);
    bool DisableVirtualization();

    void Orientation(winrt::Orientation const& value);
    winrt::Orientation Orientation();

    void Spacing(double value);
    double Spacing();

    static winrt::DependencyProperty DisableVirtualizationProperty() { return s_DisableVirtualizationProperty; }
    static winrt::DependencyProperty OrientationProperty() { return s_OrientationProperty; }
    static winrt::DependencyProperty SpacingProperty() { return s_SpacingProperty; }

    static GlobalDependencyProperty s_DisableVirtualizationProperty;
    static GlobalDependencyProperty s_OrientationProperty;
    static GlobalDependencyProperty s_SpacingProperty;

    static void EnsureProperties();
    static void ClearProperties();

    static void OnDisableVirtualizationPropertyChanged(
        winrt::DependencyObject const& sender,
        winrt::DependencyPropertyChangedEventArgs const& args);

    static void OnOrientationPropertyChanged(
        winrt::DependencyObject const& sender,
        winrt::DependencyPropertyChangedEventArgs const& args);

    static void OnSpacingPropertyChanged(
        winrt::DependencyObject const& sender,
        winrt::DependencyPropertyChangedEventArgs const& args);
};
