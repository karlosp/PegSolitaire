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
enum PositionType
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
  MoveFromParent(PositionPatch const& first,
                 PositionPatch const& second,
                 PositionPatch const& third)
  {
    patches_.push_back(first);
    patches_.push_back(second);
    patches_.push_back(third);
  }

  std::vector<PositionPatch> const& get_move() const
  {
    return patches_;
  }

  private:
  std::vector<PositionPatch> patches_;
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

  int peg_count() const
  {
    return peg_counter_;
  }

  void set_parent(std::shared_ptr<Board> parent, MoveFromParent const& move_from_parent)
  {
    parent_ = parent;

    PegPositionContainer patched_board = parent->get_positions();

    for(auto& patch : move_from_parent.get_move())
    {
      patched_board[patch.row][patch.col] = patch.type;
    }

    set_positions(std::move(patched_board), parent->peg_count() - 1);

    // Starting peg position
    from_row = move_from_parent.get_move().front().row;
    from_col = move_from_parent.get_move().front().col;

    // Last patch position it where we move peg
    to_row = move_from_parent.get_move().back().row;
    to_col = move_from_parent.get_move().back().col;
  }

  std::shared_ptr<Board> get_parent()
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
        if(row == from_row && col == from_col)
        {
          std::cout << "*";
        }
        else if(row == to_row && col == to_col)
        {
          std::cout << "X";
        }
        else
        {
          auto const cell = positions_[row][col];
          if(cell == PositionType::Peg)
            std::cout << ".";
          else if(cell == PositionType::NoP)
            std::cout << "o";
          else if(cell == PositionType::Inv)
            std::cout << " ";
        }
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

  int from_row = -1;
  int to_row = -1;
  int from_col = -1;
  int to_col = -1;
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
  std::deque<std::shared_ptr<Board>> positions{start_board};
  std::deque<std::shared_ptr<Board>> solutions;

  std::optional<int> patched_peg_count;

#ifdef ENABLE_TIME
  auto start = high_resolution_clock::now();
  const auto beggining = start;
#endif // ENABLE_TIME

  std::vector<MoveFromParent> moves_from_parent;
  moves_from_parent.reserve(16);

  while(!positions.empty() &&
#ifdef ENABLE_TIME
        solutions.size() != 1072778
#else
        solutions.empty()
#endif
  )
  {
    const auto parent_position = positions.back();
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
        // can move rigth
        if(col < 5 && board[row][col + 1] == PositionType::Peg &&
           board[row][col + 2] == PositionType::NoP)
        {
          moves_from_parent.emplace_back(
              MoveFromParent(PositionPatch{row, col, PositionType::NoP},
                             PositionPatch{row, col + 1, PositionType::NoP},
                             PositionPatch{row, col + 2, PositionType::Peg}));
        }
        // can move left
        else if(col > 1 && board[row][col - 1] == PositionType::Peg &&
                board[row][col - 2] == PositionType::NoP)
        {
          moves_from_parent.emplace_back(
              MoveFromParent(PositionPatch{row, col, PositionType::NoP},
                             PositionPatch{row, col - 1, PositionType::NoP},
                             PositionPatch{row, col - 2, PositionType::Peg}));
        }
        // can move down
        else if(row < 5 && board[row + 1][col] == PositionType::Peg &&
                board[row + 2][col] == PositionType::NoP)
        {
          moves_from_parent.emplace_back(
              MoveFromParent(PositionPatch{row, col, PositionType::NoP},
                             PositionPatch{row + 1, col, PositionType::NoP},
                             PositionPatch{row + 2, col, PositionType::Peg}));
        }
        // can move up
        else if(row > 1 && board[row - 1][col] == PositionType::Peg &&
                board[row - 2][col] == PositionType::NoP)
        {
          moves_from_parent.emplace_back(
              MoveFromParent(PositionPatch{row, col, PositionType::NoP},
                             PositionPatch{row - 1, col, PositionType::NoP},
                             PositionPatch{row - 2, col, PositionType::Peg}));
        }
      } //for
    } //for

    //if(move_from_parent)
    for(auto const& move_from_parent : moves_from_parent)
    {
      auto peg_position = std::make_shared<Board>();
      peg_position->set_parent(parent_position, move_from_parent);

      if(peg_position->peg_count() == 1)
      {
        solutions.push_back(std::move(peg_position));
      }
      else
      {
        positions.push_back(peg_position);
      }
      ++counter;
    }

    moves_from_parent.clear();

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
