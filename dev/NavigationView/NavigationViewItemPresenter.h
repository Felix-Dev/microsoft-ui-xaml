﻿// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#pragma once
#include "NavigationViewItemPresenter.g.h"
#include "NavigationViewHelper.h"
#include "NavigationViewItemPresenter.properties.h"

class NavigationViewItem;

class NavigationViewItemPresenter:
    public ReferenceTracker<NavigationViewItemPresenter, winrt::implementation::NavigationViewItemPresenterT>,
    public NavigationViewItemPresenterProperties
{
public:
    NavigationViewItemPresenter();

    // IFrameworkElementOverrides
    void OnApplyTemplate();

    // IFrameworkElementOverrides
    bool GoToElementStateCore(winrt::hstring const& stateName, bool useTransitions);

    winrt::UIElement GetSelectionIndicator();

    void UpdateContentLeftIndentation(double leftIndentation);

private:
    NavigationViewItem * GetNavigationViewItem();
    void UpdateMargin();

    NavigationViewItemHelper<NavigationViewItemPresenter> m_helper{ this };
    tracker_ref<winrt::Grid> m_contentGrid{ this };

    double m_leftIndentation{ 0 };
};
