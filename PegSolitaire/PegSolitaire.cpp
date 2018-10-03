// clang-format off
#include "stdafx.h"
// clang-format on 
#include "PegSolitaire.hpp"
#include <chrono>

int main()
{
  using namespace std::chrono;
  auto const english_board = ps::get_english_board();

  english_board->print();

  std::vector<ps::Board*> steps;

  auto const start = high_resolution_clock::now();
  int loop = 100;
  for(int i = 0; i < loop; ++i)
  {
    steps = ps::solve(english_board);
  }
  auto end = high_resolution_clock::now();

  ps::print_steps(steps);
  auto duration_ms = duration_cast<duration<double, std::milli>>(end - start).count() / loop;
  std::cout << "Solving took: " << duration_ms << " ms\n";

  system("pause");
  return 0;
}
