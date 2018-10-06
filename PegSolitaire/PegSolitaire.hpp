#pragma once
#include <array>
#include <chrono>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <optional>
#include <vector>
#include <windows.h>

//#define ENABLE_TIME TRUE;
//#define DEBUG

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

class Board;
std::vector<ps::Board*> recycled;

class Board
{
  public:
  Board() = default;
  Board& operator=(Board&& other) = default;
  Board& operator=(const Board&) = default;
  Board(const Board&) = default;

  void remove_reference()
  {
    --ref_count_;
    if(ref_count_ == 0)
    {
      if(parent_)
      {
        parent_->remove_reference();
        parent_ = nullptr;
      }
      recycled.push_back(this);
      //delete this;
    }
  }

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

  void set_parent(Board* parent,
                  PositionPatch const& patch0,
                  PositionPatch const& patch1,
                  PositionPatch const& patch2)
  {
    parent_ = parent;
    parent_->ref_count_++;

    PegPositionContainer patched_board = parent->get_positions();

    patched_board[patch0.row][patch0.col] = patch0.type;
    patched_board[patch1.row][patch1.col] = patch1.type;
    patched_board[patch2.row][patch2.col] = patch2.type;

    set_positions(std::move(patched_board), parent->peg_count() - 1);
  }

  Board* get_parent() const
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

  char ref_count_ = 0;

  private:
  PegPositionContainer positions_;
  Board* parent_ = nullptr;
  int peg_counter_ = 0;
};

std::unique_ptr<Board> get_english_board()
{
  PegPositionContainer positions{std::array<PositionType, 7>{Inv, Inv, Peg, Peg, Peg, Inv, Inv},
                                 std::array<PositionType, 7>{Inv, Inv, Peg, Peg, Peg, Inv, Inv},
                                 std::array<PositionType, 7>{Peg, Peg, Peg, Peg, Peg, Peg, Peg},
                                 std::array<PositionType, 7>{Peg, Peg, Peg, NoP, Peg, Peg, Peg},
                                 std::array<PositionType, 7>{Peg, Peg, Peg, Peg, Peg, Peg, Peg},
                                 std::array<PositionType, 7>{Inv, Inv, Peg, Peg, Peg, Inv, Inv},
                                 std::array<PositionType, 7>{Inv, Inv, Peg, Peg, Peg, Inv, Inv}};

  auto board = std::make_unique<Board>();
  board->set_positions(std::move(positions), 32);

  return std::move(board);
}
int new_board_count = 0;
Board* get_new_board()
{

  if(!recycled.empty())
  {
    auto new_board = recycled.back();
    recycled.pop_back();
    new_board->ref_count_ = 0;
    return new_board;
  }
  else
  {
    ++new_board_count;
    return new Board();
  }
}

static std::vector<ps::Board> boards_pool(148, ps::Board());

inline void helper_moving_peg_position(ps::Board*& peg_position,
                                std::vector<ps::Board*>& positions,
                                std::vector<ps::Board*>& solutions,
                                long long& counter)
{
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

inline void can_move_left(int col,
                   const ps::PegPositionContainer& board,
                   int row,
                   ps::Board* const& parent_position,
                   std::vector<ps::Board*>& positions,
                   std::vector<ps::Board*>& solutions,
                   long long& counter)
{
  if(col > 1 && board[row][col - 1] == PositionType::Peg &&
     board[row][col - 2] == PositionType::NoP)
  {
    auto peg_position = get_new_board();
    peg_position->set_parent(parent_position,
                             PositionPatch{row, col, PositionType::NoP},
                             PositionPatch{row, col - 1, PositionType::NoP},
                             PositionPatch{row, col - 2, PositionType::Peg});

    helper_moving_peg_position(peg_position, positions, solutions, counter);
  }
}
inline void can_move_down(int row,
                   const ps::PegPositionContainer& board,
                   int col,
                   ps::Board* const& parent_position,
                   std::vector<ps::Board*>& positions,
                   std::vector<ps::Board*>& solutions,
                   long long& counter)
{
  if(row < 5 && board[row + 1][col] == PositionType::Peg &&
     board[row + 2][col] == PositionType::NoP)
  {
    auto peg_position = get_new_board();
    peg_position->set_parent(parent_position,
                             PositionPatch{row, col, PositionType::NoP},
                             PositionPatch{row + 1, col, PositionType::NoP},
                             PositionPatch{row + 2, col, PositionType::Peg});

    helper_moving_peg_position(peg_position, positions, solutions, counter);
  }
}
inline void can_move_up(int row,
                 const ps::PegPositionContainer& board,
                 int col,
                 ps::Board* const& parent_position,
                 std::vector<ps::Board*>& positions,
                 std::vector<ps::Board*>& solutions,
                 long long& counter)
{
  if(row > 1 && board[row - 1][col] == PositionType::Peg &&
     board[row - 2][col] == PositionType::NoP)
  {
    auto peg_position = get_new_board();
    peg_position->set_parent(parent_position,
                             PositionPatch{row, col, PositionType::NoP},
                             PositionPatch{row - 1, col, PositionType::NoP},
                             PositionPatch{row - 2, col, PositionType::Peg});

    helper_moving_peg_position(peg_position, positions, solutions, counter);
  }
}
inline void can_move_right(int col,
                    const ps::PegPositionContainer& board,
                    int row,
                    ps::Board* const& parent_position,
                    std::vector<ps::Board*>& positions,
                    std::vector<ps::Board*>& solutions,
                    long long& counter)
{
  if(col < 5 && board[row][col + 1] == PositionType::Peg &&
     board[row][col + 2] == PositionType::NoP)
  {
    auto peg_position = get_new_board();
    peg_position->set_parent(parent_position,
                             PositionPatch{row, col, PositionType::NoP},
                             PositionPatch{row, col + 1, PositionType::NoP},
                             PositionPatch{row, col + 2, PositionType::Peg});

    helper_moving_peg_position(peg_position, positions, solutions, counter);
  }
}
std::vector<Board> solve(Board board)
{
  using namespace std::chrono;
  recycled.clear();
  for(auto& board : boards_pool)
  {
    recycled.push_back(&board);
  }
  std::cout.imbue(std::locale(""));
  long long counter{0};

  std::vector<Board*> positions;
  positions.push_back(&board);
  std::vector<Board*> solutions;

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
    bool has_child = false;
    auto current_counter = counter;
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
        can_move_left(col, board, row, parent_position, positions, solutions, counter);
        // can move down
        can_move_down(row, board, col, parent_position, positions, solutions, counter);
        // can move up
        can_move_up(row, board, col, parent_position, positions, solutions, counter);
        // can move rigth
        can_move_right(col, board, row, parent_position, positions, solutions, counter);
      } //for
    } //for

    has_child = current_counter < counter;

    if(!has_child)
    {
      // Instead of deleting we can recycle it!
      auto parent = parent_position->get_parent();
      if(parent)
      {
        parent->remove_reference();
      }
      recycled.push_back(parent_position);
    }
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

  std::vector<Board> results;

  if(!solutions.empty())
  {
    Board* current_position = solutions.back();

    while(current_position->get_parent())
    {
      results.emplace_back(*current_position);
      current_position = current_position->get_parent();
    }
  }
  std::reverse(results.begin(), results.end());

#if defined DEBUG
  std::cout << "counter " << counter << "\n";
  std::cout << "new board count " << new_board_count << "\n";
  std::cout << "solutions size: " << solutions.size() << "\n";
#endif
  return results;
}

template <class T>
void print_steps(T const& steps)
{
  for(auto const& board : steps)
  {
    board.print();
  }
}

} // namespace ps
