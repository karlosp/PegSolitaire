#pragma once
#include <array>
#include <chrono>
#include <deque>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>
#include <windows.h>

//#define ENABLE_TIME TRUE;

namespace
{
enum PositionType : char
{
  Inv /*Invalid*/,
  NoP /*No Peg*/,
  Peg
};

struct PositionPatch
{
  int row = 0;
  int col = 0;
  PositionType type;
};

class MoveFromParent
{
  public:
  MoveFromParent(PositionPatch&& first, PositionPatch&& second, PositionPatch&& third)
  {
    patches_[0] = std::move(first);
    patches_[1] = std::move(second);
    patches_[2] = std::move(third);
  }

  std::array<PositionPatch, 3> const& get_move() const
  {
    return patches_;
  }

  private:
  std::array<PositionPatch, 3> patches_;
};
} // namespace

namespace ps
{

using PegPositionContainer = std::array<std::array<PositionType, 7>, 7>;

class Board
{
  public:
  void set_positions(PegPositionContainer&& new_positions, int peg_count)
  {
    positions_ = std::move(new_positions);
    peg_counter_ = peg_count;
  }

  PegPositionContainer const& get_positions() const
  {
    return positions_;
  }

  inline int peg_count() const
  {
    return peg_counter_;
  }

  void set_parent(std::shared_ptr<Board> parent,
                  PositionPatch const& patch0,
                  PositionPatch const& patch1,
                  PositionPatch const& patch2)
  {
    parent_ = parent;

    PegPositionContainer patched_board = parent->get_positions();

    patched_board[patch0.row][patch0.col] = patch0.type;
    patched_board[patch1.row][patch1.col] = patch1.type;
    patched_board[patch2.row][patch2.col] = patch2.type;

    set_positions(std::move(patched_board), parent->peg_count() - 1);
  }

  std::shared_ptr<Board> get_parent() const
  {
    return parent_;
  }

  void print() const
  {
    SetConsoleOutputCP(65001);
    std::cout << "~~~~~~~~~~~~~~~\n\n";

    for(auto row = 0u; row < positions_.size(); ++row)
    {
      for(auto col = 0u; col < positions_.front().size(); ++col)
      {
        auto const cell = positions_[row][col];
        if(cell == PositionType::Peg)
          std::cout << ".";
        else if(cell == PositionType::NoP)
          std::cout << "o";
        else if(cell == PositionType::Inv)
          std::cout << " ";
      }
      std::cout << "\n";
    }
    std::cout << "\n";
    std::cout << "Peg count: " << peg_counter_ << "\n\n";
  }

  private:
  PegPositionContainer positions_;
  std::shared_ptr<Board> parent_ = nullptr;
  int peg_counter_ = 0;
};

Board get_english_board()
{
  PegPositionContainer positions{std::array<PositionType, 7>{Inv, Inv, Peg, Peg, Peg, Inv, Inv},
                                 std::array<PositionType, 7>{Inv, Inv, Peg, Peg, Peg, Inv, Inv},
                                 std::array<PositionType, 7>{Peg, Peg, Peg, Peg, Peg, Peg, Peg},
                                 std::array<PositionType, 7>{Peg, Peg, Peg, NoP, Peg, Peg, Peg},
                                 std::array<PositionType, 7>{Peg, Peg, Peg, Peg, Peg, Peg, Peg},
                                 std::array<PositionType, 7>{Inv, Inv, Peg, Peg, Peg, Inv, Inv},
                                 std::array<PositionType, 7>{Inv, Inv, Peg, Peg, Peg, Inv, Inv}};

  Board board;
  board.set_positions(std::move(positions), 32);

  return board;
}

std::vector<std::shared_ptr<Board>> solve(Board const& board)
{
  using namespace std::chrono;
  std::cout.imbue(std::locale(""));
  long long counter{0};
  auto start_board = std::make_shared<Board>();
  *start_board = board;
  std::deque<std::shared_ptr<Board> /*, Moya::Allocator<std::shared_ptr<Board>>*/> positions;
  positions.emplace_back(std::move(start_board));
  //std::deque<std::shared_ptr<Board>> positions{start_board};
  std::deque<std::shared_ptr<Board>> solutions;

  std::optional<int> patched_peg_count;

#ifdef ENABLE_TIME
  auto start = high_resolution_clock::now();
  const auto beggining = start;
#endif // ENABLE_TIME



  while(!positions.empty() &&
#ifdef ENABLE_TIME
        solutions.size() != 1072778
#else
        solutions.empty()
#endif
  )
  {
    const auto parent_position = positions.back();
    // Copy to stack to be faster
    const auto board = parent_position->get_positions();
    positions.pop_back();

    // generate all possible positions
    for(auto row = 0; row < 7; ++row)
    {
      for(auto col = 0; col < 7; ++col)
      {
        if(board[row][col] != PositionType::Peg)
        {
          continue;
        }

        // can move left
        if(col > 1 && board[row][col - 1] == PositionType::Peg &&
           board[row][col - 2] == PositionType::NoP)
        {
          auto peg_position = std::make_shared<Board>();
          peg_position->set_parent(parent_position,
                                   PositionPatch{row, col, PositionType::NoP},
                                   PositionPatch{row, col - 1, PositionType::NoP},
                                   PositionPatch{row, col - 2, PositionType::Peg});

          if(peg_position->peg_count() != 1)
          {
            positions.emplace_back(std::move(peg_position));
          }
          else
          {
            solutions.emplace_back(std::move(peg_position));
          }
          ++counter;
        }
        // can move down
        if(row < 5 && board[row + 1][col] == PositionType::Peg &&
           board[row + 2][col] == PositionType::NoP)
        {
          auto peg_position = std::make_shared<Board>();
          peg_position->set_parent(parent_position,
                                   PositionPatch{row, col, PositionType::NoP},
                                   PositionPatch{row + 1, col, PositionType::NoP},
                                   PositionPatch{row + 2, col, PositionType::Peg});

          if(peg_position->peg_count() != 1)
          {
            positions.emplace_back(std::move(peg_position));
          }
          else
          {
            solutions.emplace_back(std::move(peg_position));
          }
          ++counter;
        }
        // can move up
        if(row > 1 && board[row - 1][col] == PositionType::Peg &&
           board[row - 2][col] == PositionType::NoP)
        {
          auto peg_position = std::make_shared<Board>();
          peg_position->set_parent(parent_position,
                                   PositionPatch{row, col, PositionType::NoP},
                                   PositionPatch{row - 1, col, PositionType::NoP},
                                   PositionPatch{row - 2, col, PositionType::Peg});

          if(peg_position->peg_count() != 1)
          {
            positions.emplace_back(std::move(peg_position));
          }
          else
          {
            solutions.emplace_back(std::move(peg_position));
          }
          ++counter;
        }
        // can move rigth
        if(col < 5 && board[row][col + 1] == PositionType::Peg &&
           board[row][col + 2] == PositionType::NoP)
        {
          auto peg_position = std::make_shared<Board>();
          peg_position->set_parent(parent_position,
                                   PositionPatch{row, col, PositionType::NoP},
                                   PositionPatch{row, col + 1, PositionType::NoP},
                                   PositionPatch{row, col + 2, PositionType::Peg});

          if(peg_position->peg_count() != 1)
          {
            positions.emplace_back(std::move(peg_position));
          }
          else
          {
            solutions.emplace_back(std::move(peg_position));
          }
          ++counter;
        }
      } //for
    } //for

#ifdef ENABLE_TIME
    auto const now = high_resolution_clock::now();
    if((now - start) > std::chrono::seconds(2))
    {
      start = now;
      std::cout << "Lapsed: " << duration_cast<seconds>(now - beggining).count() << " s\n";
      std::cout << "Positions counter " << positions.size() << "\n";
      std::cout << "Last position pegs count " << positions.back()->peg_count() << "\n";
      std::cout << "Moves counter " << counter << " \n";
      std::cout << "Solutions size: " << solutions.size() << " \n\n";
    }
#endif
  }
#ifdef ENABLE_TIME
  auto const now = high_resolution_clock::now();
  std::cout << "Lapsed time for all solutions: " << duration_cast<seconds>(now - start).count()
            << " s\n";
#endif

  std::vector<std::shared_ptr<Board>> results;

  if(!solutions.empty())
  {
    auto current_position = solutions.back();

    while(current_position->get_parent())
    {
      results.push_back(current_position);
      current_position = current_position->get_parent();
    }
  }
  std::reverse(results.begin(), results.end());
  std::cout << "counter " << counter << "\n";
  std::cout << "solutions size: " << solutions.size() << "\n";
  return results;
}

void print_steps(std::vector<std::shared_ptr<Board>> const& steps)
{
  for(auto const& board : steps)
  {
    board->print();
  }
}

} // namespace ps
