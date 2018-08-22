// PegSolitaire.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "PegSolitaire.hpp"
#include <chrono>

int main()
{
  using namespace std::chrono;
  auto const english_board = ps::get_english_board();

  ps::print(english_board);

  

  auto  steps = ps::solve(english_board);

  auto start = high_resolution_clock::now();
  int loop = 100;
  for (int i = 0; i < loop; ++i)
    steps = ps::solve(english_board);
  auto end = high_resolution_clock::now();

  ps::print_steps(steps);
  auto duration_ms = duration_cast<duration<double, std::milli>>(end - start).count() / loop;
  std::cout << "Solving took: " << duration_ms << " ms\n";

  system("pause");
  return 0;
}

