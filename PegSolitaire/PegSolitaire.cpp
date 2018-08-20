// PegSolitaire.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "PegSolitaire.hpp"
#include <chrono>

int main()
{
  using namespace std::chrono;
  auto const english_position = ps::get_english_position();

  ps::print_position(english_position);

  std::cout << "Peg count: " << ps::count_pegs(english_position.board) << "\n";

  auto  steps = ps::solve_peg_solitaire(english_position);

  auto start = high_resolution_clock::now();
  int loop = 10;
  for (int i = 0; i < loop; ++i)
    steps = ps::solve_peg_solitaire(english_position);
  auto end = high_resolution_clock::now();

  ps::print_steps(steps);
  std::cout << "Solving took: " << duration_cast<duration<double>>(end - start).count() / loop << " s\n";
  return 0;
}

