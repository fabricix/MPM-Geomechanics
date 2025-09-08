// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include <gtest/gtest.h>
#include <iostream>
#include "configuration.performance.h"

using namespace std;

string getFilters()
{
    string filter = "";

    cout << "----> Test Filters <----\n" << endl;
    if (Configuration::interpolationTest)
    {
        cout << "----> Interpolation test: enabled" << endl;
        filter += "Interpolation*";
    }
    else
    {
        cout << "----> Interpolation test: disabled" << endl;
    }

    if (Configuration::updateTest)
    {
        cout << "----> Update test: enabled" << endl;
        if (!filter.empty()) filter += ":";
        filter += "Update*";
    }
    else
    {
        cout << "----> Update test: disabled" << endl;
    }
    cout << "\n----> Test Filters <----\n" << endl;

    return filter;
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    int status = Configuration::setup();

    if (status != 0) {
        return status;
    }
    else
    {
        // Get test filters
        string filter = getFilters();

        // Set GoogleTest filter
        if (!filter.empty()) { ::testing::GTEST_FLAG(filter) = filter; }

        cout << "Running tests..." << endl;
        return RUN_ALL_TESTS();
    }
}

