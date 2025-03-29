#include <gtest/gtest.h>
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
  cout << "Main!" << endl;
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}