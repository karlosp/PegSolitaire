// PegSolitaire.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "PegSolitaire.hpp"

int main()
{
  auto const english_position = ps::get_english_position();

  ps::print_position(english_position);

  auto const steps = ps::solve_peg_solitaire(english_position);

  ps::print_steps(steps);

  return 0;
}

