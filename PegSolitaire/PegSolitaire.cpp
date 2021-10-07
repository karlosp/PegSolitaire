// clang-format off
#include "stdafx.h"
// clang-format on 
#include "PegSolitaire.hpp"
#include <chrono>
#include <array>
#include <vector>

enum  Pos {
  PEG = 1,
  NOP = 0,
  INV = 8
};

struct Board {
  std::array<std::array<Pos,7>,7> board;
  std::size_t num_pegs;
};

std::array<Pos,7> row0156{std::array<Pos,7>{Pos::INV, Pos::INV, Pos::PEG, Pos::PEG, Pos::PEG, Pos::INV, Pos::INV}};
std::array<Pos,7> row24{std::array<Pos,7>{Pos::PEG, Pos::PEG, Pos::PEG, Pos::PEG, Pos::PEG, Pos::PEG, Pos::PEG}};
std::array<Pos,7> row3{std::array<Pos,7>{Pos::PEG, Pos::PEG, Pos::PEG, Pos::NOP, Pos::PEG, Pos::PEG, Pos::PEG}};

Board english{{row0156, row0156, row24, row3, row24, row0156, row0156},32};

void print(const Board& b) {
  for (std::size_t i = 0; i < 7u; ++i) {
    for (std::size_t j = 0; j < 7u; ++j) {
      const auto& current = b.board[i][j];
      if (current == Pos::INV) {
        std::cout << " ";
      }
      else{
        std::cout << b.board[i][j];
        }
     }
    std::cout << "\n";
  }
  std::cout << "\n";


}

std::size_t solve(Board b, std::vector<Board> &board);

std::size_t move_right(Board b, std::vector<Board>& results) {
  for (std::size_t row = 0; row < 7u; ++row) {
    for (std::size_t col = 0; col < 5u; ++col) {
      if (b.board[row][col] == Pos::PEG && b.board[row][col + 1] == Pos::PEG && b.board[row][col + 2] == Pos::NOP) {
        b.board[row][col] = Pos::NOP;
        b.board[row][col+1]= Pos::NOP;
        b.board[row][col+2] = Pos::PEG;
        b.num_pegs--;
        return solve(b,results);
      }
     }
    
  }
  return b.num_pegs;
}


std::size_t move_left(Board b, std::vector<Board>& results) {
  for (std::size_t row = 0; row < 7u; ++row) {
    for (std::size_t col = 2; col < 7u; ++col) {
      if (b.board[row][col] == Pos::PEG && b.board[row][col - 1] == Pos::PEG && b.board[row][col - 2] == Pos::NOP) {
        b.board[row][col] = Pos::NOP;
        b.board[row][col-1]= Pos::NOP;
        b.board[row][col-2] = Pos::PEG;
        b.num_pegs--;
        return solve(b, results);
      }
     }
    
  }
    return b.num_pegs;
}

std::size_t move_down(Board b, std::vector<Board>& results) {
  for (std::size_t row = 0; row < 5u; ++row) {
    for (std::size_t col = 0; col < 7u; ++col) {
      if (b.board[row][col] == Pos::PEG && b.board[row+1][col] == Pos::PEG && b.board[row+2][col] == Pos::NOP) {
        b.board[row][col] = Pos::NOP;
        b.board[row+1][col]= Pos::NOP;
        b.board[row+2][col] = Pos::PEG;
        b.num_pegs--;
        return solve(b, results);
      }
     }
    
  }
    return b.num_pegs;
}

std::size_t move_up(Board b, std::vector<Board>& results) {
  for (std::size_t row = 2; row < 7u; ++row) {
    for (std::size_t col = 0; col < 7u; ++col) {
      if (b.board[row][col] == Pos::PEG && b.board[row-1][col] == Pos::PEG && b.board[row-2][col] == Pos::NOP) {
        b.board[row][col] = Pos::NOP;
        b.board[row-1][col]= Pos::NOP;
        b.board[row-2][col] = Pos::PEG;
        b.num_pegs--;
        return solve(b, results);
      }
     }
    
  }
  return b.num_pegs;
}


std::size_t solve(Board b, std::vector<Board>& results) {
  if (b.num_pegs == 1) {
    //print(b);
    results.push_back(b);
    return 1;
  }
 
  auto pegs_left = 0;

  pegs_left = move_left(b,results);
  if (pegs_left == 1) {
    //print(b);
    results.push_back(b);
    return 1;
  }

     pegs_left = move_down(b, results);
  if (pegs_left == 1) {
    //print(b);
    results.push_back(b);
    return 1;
  }
  
    pegs_left = move_up(b, results);
  if (pegs_left == 1) {
    //print(b);
    results.push_back(b);
    return 1;
  }


  pegs_left = move_right(b,results);
  if (pegs_left == 1) {
    //print(b);
    results.push_back(b);
    return 1;
  }

  return pegs_left;
}

void print_results(const std::vector<Board>& results) {
  for (auto it = results.crbegin(); it != results.crend(); it++) {
    print(*it);
  }
}

int main()
{
  using namespace std::chrono;
  auto const english_board = ps::get_english_board();

  std::vector<Board> results;
  auto const start = high_resolution_clock::now();
  solve(english, results);
  auto end = high_resolution_clock::now();
  auto duration_ms = duration_cast<duration<double, std::milli>>(end - start).count();

  print_results(results);
  std::cout << "Solving took: " << duration_ms << " ms\n";

  return 0;
  /*
  english_board->print();

  std::vector<ps::Board> steps;

  auto const start = high_resolution_clock::now();
  int loop = 1000;
  for(int i = 0; i < loop; ++i)
  {
    steps = ps::solve(*english_board.get());
  }
  auto end = high_resolution_clock::now();

  ps::print_steps(steps);
  auto duration_ms = duration_cast<duration<double, std::milli>>(end - start).count() / loop;
  std::cout << "Solving took: " << duration_ms << " ms\n";

  system("pause");

  return 0;
  */
}
