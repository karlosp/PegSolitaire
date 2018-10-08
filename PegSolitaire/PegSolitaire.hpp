#pragma once
#include <array>
#include <chrono>
#include <deque>
#include <iostream>
#include <list>
#include <memory>
#include <optional>
#include <unordered_set>
#include <vector>
#include <windows.h>

//#define ENABLE_TIME TRUE;
//#define DEBUG

namespace
{
enum PositionType : int
{
  Inv /*Invalid*/,
  NoP /*No Peg*/,
  Peg
};

struct PositionPatch
{
  int index = 0;
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

using PegPositionContainer = std::array<PositionType, 49>;

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

    patched_board[patch0.index] = patch0.type;
    patched_board[patch1.index] = patch1.type;
    patched_board[patch2.index] = patch2.type;

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
    for(auto counter = 0u; counter < positions_.size(); ++counter)
    {
      int row = std::ceil(counter / 7);
      int col = counter % 7;
      auto const cell = positions_[counter];
      if(cell == PositionType::Peg)
        std::cout << ".";
      else if(cell == PositionType::NoP)
        std::cout << "o";
      else if(cell == PositionType::Inv)
        std::cout << " ";

      if((counter + 1) % 7 == 0)
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
  PegPositionContainer positions {std::array<PositionType, 49> {
      Inv, Inv, Peg, Peg, Peg, Inv, Inv, Inv, Inv, Peg, Peg, Peg, Inv, Inv, Peg, Peg, Peg,
      Peg, Peg, Peg, Peg, Peg, Peg, Peg, NoP, Peg, Peg, Peg, Peg, Peg, Peg, Peg, Peg, Peg,
      Peg, Inv, Inv, Peg, Peg, Peg, Inv, Inv, Inv, Inv, Peg, Peg, Peg, Inv, Inv}};

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
                                       std::vector<ps::Board*>& solutions)
{
  if(peg_position->peg_count() != 1)
  {
    positions.emplace_back(std::move(peg_position));
  }
  else
  {
    solutions.emplace_back(std::move(peg_position));
  }
}

inline void can_move_left(int index,
                          const ps::PegPositionContainer& board,
                          ps::Board* const& parent_position,
                          std::vector<ps::Board*>& positions,
                          std::vector<ps::Board*>& solutions,
                          long long& counter)
{
  if(board[index - 1] == PositionType::Peg && board[index - 2] == PositionType::NoP)
  {
    auto peg_position = get_new_board();
    peg_position->set_parent(parent_position,
                             PositionPatch {index, PositionType::NoP},
                             PositionPatch {index - 1, PositionType::NoP},
                             PositionPatch {index - 2, PositionType::Peg});

    helper_moving_peg_position(peg_position, positions, solutions);
    ++counter;
  }
}
inline void can_move_down(int index,
                          const ps::PegPositionContainer& board,
                          ps::Board* const& parent_position,
                          std::vector<ps::Board*>& positions,
                          std::vector<ps::Board*>& solutions,
                          long long& counter)
{
  if(board[index + 7] == PositionType::Peg && board[index + 14] == PositionType::NoP)
  {
    auto peg_position = get_new_board();
    peg_position->set_parent(parent_position,
                             PositionPatch {index, PositionType::NoP},
                             PositionPatch {index + 7, PositionType::NoP},
                             PositionPatch {index + 14, PositionType::Peg});

    helper_moving_peg_position(peg_position, positions, solutions);
    ++counter;
  }
}
inline void can_move_up(int index,
                        const ps::PegPositionContainer& board,
                        ps::Board* const& parent_position,
                        std::vector<ps::Board*>& positions,
                        std::vector<ps::Board*>& solutions,
                        long long& counter)
{
  if(board[index - 7] == PositionType::Peg && board[index - 14] == PositionType::NoP)
  {
    auto peg_position = get_new_board();
    peg_position->set_parent(parent_position,
                             PositionPatch {index, PositionType::NoP},
                             PositionPatch {index - 7, PositionType::NoP},
                             PositionPatch {index - 14, PositionType::Peg});

    helper_moving_peg_position(peg_position, positions, solutions);
    ++counter;
  }
}
inline void can_move_right(int index,
                           const ps::PegPositionContainer& board,
                           ps::Board* const& parent_position,
                           std::vector<ps::Board*>& positions,
                           std::vector<ps::Board*>& solutions,
                           long long& counter)
{
  if(board[index + 1] == PositionType::Peg && board[index + 2] == PositionType::NoP)
  {
    auto peg_position = get_new_board();
    peg_position->set_parent(parent_position,
                             PositionPatch {index, PositionType::NoP},
                             PositionPatch {index + 1, PositionType::NoP},
                             PositionPatch {index + 2, PositionType::Peg});

    helper_moving_peg_position(peg_position, positions, solutions);
    ++counter;
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
#if defined DEBUG
  std::cout.imbue(std::locale(""));
#endif
  long long counter {0};

  std::vector<Board*> positions;
  positions.push_back(&board);
  std::vector<Board*> solutions;
  // clang-format of
  const std::vector<int> row_cols {2,  3,  4,  9,  10, 11, 14, 15, 16, 17, 18,
                                   19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
                                   30, 31, 32, 33, 34, 37, 38, 39, 44, 45, 46};

  //clang-format on
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
    //for(auto& row_col : row_cols)
    //{
    int row_col = 2;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_right(row_col, board, parent_position, positions, solutions, counter);
      can_move_down(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 3;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_down(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 4;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_left(row_col, board, parent_position, positions, solutions, counter);
      can_move_down(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 9;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_right(row_col, board, parent_position, positions, solutions, counter);
      can_move_down(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 10;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_down(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 11;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_left(row_col, board, parent_position, positions, solutions, counter);
      can_move_down(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 14;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_down(row_col, board, parent_position, positions, solutions, counter);
      can_move_right(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 15;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_down(row_col, board, parent_position, positions, solutions, counter);
      can_move_right(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 16;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_left(row_col, board, parent_position, positions, solutions, counter);
      can_move_down(row_col, board, parent_position, positions, solutions, counter);
      can_move_up(row_col, board, parent_position, positions, solutions, counter);
      can_move_right(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 17;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_left(row_col, board, parent_position, positions, solutions, counter);
      can_move_down(row_col, board, parent_position, positions, solutions, counter);
      can_move_up(row_col, board, parent_position, positions, solutions, counter);
      can_move_right(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 18;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_left(row_col, board, parent_position, positions, solutions, counter);
      can_move_down(row_col, board, parent_position, positions, solutions, counter);
      can_move_up(row_col, board, parent_position, positions, solutions, counter);
      can_move_right(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 19;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_left(row_col, board, parent_position, positions, solutions, counter);
      can_move_down(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 20;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_left(row_col, board, parent_position, positions, solutions, counter);
      can_move_down(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 21;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_right(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 22;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_right(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 23;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_left(row_col, board, parent_position, positions, solutions, counter);
      can_move_down(row_col, board, parent_position, positions, solutions, counter);
      can_move_up(row_col, board, parent_position, positions, solutions, counter);
      can_move_right(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 24;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_left(row_col, board, parent_position, positions, solutions, counter);
      can_move_down(row_col, board, parent_position, positions, solutions, counter);
      can_move_up(row_col, board, parent_position, positions, solutions, counter);
      can_move_right(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 25;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_left(row_col, board, parent_position, positions, solutions, counter);
      can_move_down(row_col, board, parent_position, positions, solutions, counter);
      can_move_up(row_col, board, parent_position, positions, solutions, counter);
      can_move_right(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 26;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_left(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 27;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_left(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 28;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_up(row_col, board, parent_position, positions, solutions, counter);
      can_move_right(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 29;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_up(row_col, board, parent_position, positions, solutions, counter);
      can_move_right(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 30;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_left(row_col, board, parent_position, positions, solutions, counter);
      can_move_down(row_col, board, parent_position, positions, solutions, counter);
      can_move_up(row_col, board, parent_position, positions, solutions, counter);
      can_move_right(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 31;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_left(row_col, board, parent_position, positions, solutions, counter);
      can_move_down(row_col, board, parent_position, positions, solutions, counter);
      can_move_up(row_col, board, parent_position, positions, solutions, counter);
      can_move_right(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 32;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_left(row_col, board, parent_position, positions, solutions, counter);
      can_move_down(row_col, board, parent_position, positions, solutions, counter);
      can_move_up(row_col, board, parent_position, positions, solutions, counter);
      can_move_right(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 33;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_left(row_col, board, parent_position, positions, solutions, counter);
      can_move_up(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 34;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_left(row_col, board, parent_position, positions, solutions, counter);
      can_move_up(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 37;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_up(row_col, board, parent_position, positions, solutions, counter);
      can_move_right(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 38;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_up(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 39;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_left(row_col, board, parent_position, positions, solutions, counter);
      can_move_up(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 44;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_up(row_col, board, parent_position, positions, solutions, counter);
      can_move_right(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 45;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_up(row_col, board, parent_position, positions, solutions, counter);
    }

    row_col = 46;
    if(board[row_col] == PositionType::Peg)
    {
      can_move_left(row_col, board, parent_position, positions, solutions, counter);
      can_move_up(row_col, board, parent_position, positions, solutions, counter);
    }
    //} //for

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
} // namespace ps

template <class T>
void print_steps(T const& steps)
{
  for(auto const& board : steps)
  {
    board.print();
  }
}

} // namespace ps
