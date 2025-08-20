// SPDX-License-Identifier: MIT
// Copyright (c) 2021-2025 MPM-Geomechanics Development Team

#include <gtest/gtest.h>
#include <iostream>
#include "performance/configuration.performance.h"

using namespace std;

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    int status = Configuration::setup();

    if (status != 0) {
        return status;
    }
    else {
        cout << "Running tests..." << endl;
        return RUN_ALL_TESTS();
    }

}