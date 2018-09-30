#pragma once
#include <array>
#include <chrono>
#include <deque>
#include <iostream>
#include <memory>
#include <optional>
#include <unordered_set>
#include <vector>
#include <windows.h>

#define ENABLE_TIME TRUE;

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
  MoveFromParent(char first, char second, char third)
  {
    patches_[0] = first;
    patches_[1] = second;
    patches_[2] = third;
  }

  std::array<char, 3> const& get_move() const
  {
    return patches_;
  }

  private:
  std::array<char, 3> patches_;
};
} // namespace

namespace ps
{

//using PegPositionContainer = std::array<std::array<PositionType, 7>, 7>;
using PegPositionContainer = std::unordered_set<char>;

class Board
{
  public:
  void set_positions(PegPositionContainer&& new_positions)
  {
    positions_ = std::move(new_positions);
  }

  PegPositionContainer  get_positions() const
  {
    return positions_;
  }

  inline std::size_t peg_count() const
  {
    return positions_.size();
  }

  void set_parent(std::shared_ptr<Board> parent, MoveFromParent const& move_from_parent)
  {
    parent_ = parent;

    PegPositionContainer patched_board = parent->get_positions();

    patched_board.erase(move_from_parent.get_move()[0]);
    patched_board.erase(move_from_parent.get_move()[1]);
    patched_board.emplace(move_from_parent.get_move()[2]);

    set_positions(std::move(patched_board));
  }

  std::shared_ptr<Board> get_parent() const
  {
    return parent_;
  }

  void print() const
  {
    SetConsoleOutputCP(65001);
    std::cout << "~~~~~~~~~~~~~~~\n\n";
    std::array<std::array<PositionType, 7>, 7> positions{
        std::array<PositionType, 7>{Inv, Inv, Peg, Peg, Peg, Inv, Inv},
                                   std::array<PositionType, 7>{Inv, Inv, NoP, NoP, NoP, Inv, Inv},
                                   std::array<PositionType, 7>{NoP, NoP, NoP, NoP, NoP, NoP, NoP},
                                   std::array<PositionType, 7>{NoP, NoP, NoP, NoP, NoP, NoP, NoP},
                                   std::array<PositionType, 7>{NoP, NoP, NoP, NoP, NoP, NoP, NoP},
                                   std::array<PositionType, 7>{Inv, Inv, NoP, NoP, NoP, Inv, Inv},
                                   std::array<PositionType, 7>{Inv, Inv, NoP, NoP, NoP, Inv, Inv}};

    for(auto row = 0u; row < positions.size(); ++row)
    {
      for(auto col = 0u; col < positions.front().size(); ++col)
      {
        auto const cell = positions[row][col];
        if(positions_.count(row*7+col)/* cell == PositionType::Peg*/)
          std::cout << ".";
        else if(cell == PositionType::NoP)
          std::cout << "o";
        else if(cell == PositionType::Inv)
          std::cout << " ";
      }
      std::cout << "\n";
    }
    std::cout << "\n";
    std::cout << "Peg count: " << positions_.size() << "\n\n";
  }

  private:
  PegPositionContainer positions_;
  std::shared_ptr<Board> parent_ = nullptr;
};

Board get_english_board()
{
  //PegPositionContainer positions{std::array<PositionType, 7>{Inv, Inv, Peg, Peg, Peg, Inv, Inv},
  //                               std::array<PositionType, 7>{Inv, Inv, Peg, Peg, Peg, Inv, Inv},
  //                               std::array<PositionType, 7>{Peg, Peg, Peg, Peg, Peg, Peg, Peg},
  //                               std::array<PositionType, 7>{Peg, Peg, Peg, NoP, Peg, Peg, Peg},
  //                               std::array<PositionType, 7>{Peg, Peg, Peg, Peg, Peg, Peg, Peg},
  //                               std::array<PositionType, 7>{Inv, Inv, Peg, Peg, Peg, Inv, Inv},
  //                               std::array<PositionType, 7>{Inv, Inv, Peg, Peg, Peg, Inv, Inv}};
  PegPositionContainer positions{2, 3, 4,
                                  9,10,11,
                                 14,15,16,17,18,19,20,
                                21,22,23,25,26,27,
                                28,29,30,31,32,33,34,
                                37,38,39,
                                44,45,46  };
  Board board;
  board.set_positions(std::move(positions));

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
  const std::unordered_set<char> invalid_pos{0,1,5,6,7,8,12,13,35,36,40,41,42,43,47,48};

#ifdef ENABLE_TIME
  auto start = high_resolution_clock::now();
  const auto beggining = start;
#endif // ENABLE_TIME

  std::vector<MoveFromParent> moves_from_parent;

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
    for(auto pos : board)
    {
      // can move left
      if(pos >= 4 && board.count(pos - 1) && board.count(pos - 2) == 0u && invalid_pos.count(pos-2) == 0u)
      {
        moves_from_parent.emplace_back(MoveFromParent(pos,pos-1, pos-2));
      }
      // can move down
      if(pos <= 32 && board.count(pos + 7) && board.count(pos + 14) == 0u && invalid_pos.count(pos+14) == 0u)
      {
        moves_from_parent.emplace_back(MoveFromParent(pos, pos +7, pos +14));
      }
      // can move up
      if(pos >= 16 && board.count(pos - 7) && board.count(pos - 14) == 0u && invalid_pos.count(pos-14) == 0u)
      {
        moves_from_parent.emplace_back(MoveFromParent(pos, pos - 7, pos - 14));
      }
      // can move right
      if(pos <= 44 && board.count(pos + 1) && board.count(pos + 2) == 0u && invalid_pos.count(pos+2) == 0u)
      {
        moves_from_parent.emplace_back(MoveFromParent(pos, pos + 1, pos + 2));
      }
    }

    //if(move_from_parent)
    for(auto const& move_from_parent : moves_from_parent)
    {
      auto peg_position = std::make_shared<Board>();
      peg_position->set_parent(parent_position, move_from_parent);

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
