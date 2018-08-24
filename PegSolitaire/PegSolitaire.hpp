#pragma once
#include <vector>
#include <iostream>
#include <windows.h>
#include <memory>
#include <deque>
#include <optional>

namespace {
  enum PositionType
  {
    Inv /*Invalid*/, NoP /*No Peg*/, Peg
  };

  struct PositionPatch {
    int row = 0;
    int col = 0;
    PositionType type;
  };

  class MoveFromParent {
  public:
    MoveFromParent(PositionPatch const& first, PositionPatch const& second, PositionPatch const& third) {
      patches_.push_back(first);
      patches_.push_back(second);
      patches_.push_back(third);
    }

    std::vector<PositionPatch> const& get_move() const {
      return patches_;
    }
  private:
    std::vector<PositionPatch> patches_;
  };
}

namespace ps {

  using PegPositionContainer = std::vector<std::vector<PositionType>>;

  class Board {
  public:
    void set_positions(PegPositionContainer && new_positions, std::optional<int> peg_count = std::nullopt) {
      positions_ = std::move(new_positions);

      if (peg_count) {
        peg_counter_ = peg_count.value();
      }
      else {
        peg_counter_ = 0;
        // Count pegs
        for (auto& row : positions_) {
          for (auto & col : row) {
            if (col == PositionType::Peg) { ++peg_counter_; }
          }
        }
      }
    }

    PegPositionContainer const& get_positions() const {
      return positions_;
    }

    int peg_count() const {
      return peg_counter_;
    }

    void set_parent(std::shared_ptr<Board> parent, MoveFromParent const& move_from_parent) {
      parent_ = parent;

      PegPositionContainer patched_board = parent->get_positions();;

      for (auto& patch : move_from_parent.get_move()) {
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

    std::shared_ptr<Board> get_parent() {
      return parent_;
    }

    void print() {
      SetConsoleOutputCP(65001);
      std::cout << "~~~~~~~~~~~~~~~\n\n";

      for (auto row = 0u; row < positions_.size(); ++row) {
        for (auto col = 0u; col < positions_.front().size(); ++col) {
          if (row == from_row && col == from_col) {
            std::cout << "*";
          }
          else if (row == to_row && col == to_col) {
            std::cout << "X";
          }
          else {
            auto const cell = positions_[row][col];
            if (cell == PositionType::Peg)
              std::cout << "·";
            else if (cell == PositionType::NoP)
              std::cout << "o";
            else if (cell == PositionType::Inv)
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

  std::shared_ptr<Board> get_english_board() {
    PegPositionContainer positions;
    positions.emplace_back(std::vector<PositionType>{Inv, Inv, Peg, Peg, Peg, Inv, Inv});
    positions.emplace_back(std::vector<PositionType>{Inv, Inv, Peg, Peg, Peg, Inv, Inv});
    positions.emplace_back(std::vector<PositionType>{Peg, Peg, Peg, Peg, Peg, Peg, Peg});
    positions.emplace_back(std::vector<PositionType>{Peg, Peg, Peg, NoP, Peg, Peg, Peg});
    positions.emplace_back(std::vector<PositionType>{Peg, Peg, Peg, Peg, Peg, Peg, Peg});
    positions.emplace_back(std::vector<PositionType>{Inv, Inv, Peg, Peg, Peg, Inv, Inv});
    positions.emplace_back(std::vector<PositionType>{Inv, Inv, Peg, Peg, Peg, Inv, Inv});

    auto board = std::make_shared<Board>();
    board->set_positions(std::move(positions));

    return board;
  }

  std::vector<std::shared_ptr<Board>> solve(std::shared_ptr<Board> const board) {
    std::deque<std::shared_ptr<Board>> positions{ board };

    std::optional<int> patched_peg_count;
    while (!positions.empty() && (*positions.back()).peg_count() != 1) {
      const auto parent_position = positions.back();
      const auto board = parent_position->get_positions();
      positions.pop_back();

      std::optional<MoveFromParent> move_from_parent = std::nullopt;
      // generate all possible positions
      for (auto row = 0; row < (int)board.size(); ++row) {
        for (auto col = 0; col < (int)board.front().size(); ++col) {

          // can move rigth
          if (col < 5 &&
            board[row][col] == PositionType::Peg &&
            board[row][col + 1] == PositionType::Peg &&
            board[row][col + 2] == PositionType::NoP) {
            // Execute right move
            move_from_parent = std::make_optional<MoveFromParent>(PositionPatch{ row, col, PositionType::NoP },
              PositionPatch{ row, col, PositionType::NoP },
              PositionPatch{ row, col + 2, PositionType::Peg });
          }
          // can move left
          else if (col > 1 &&
            board[row][col] == PositionType::Peg &&
            board[row][col - 1] == PositionType::Peg &&
            board[row][col - 2] == PositionType::NoP) {
            // Execute right move
            move_from_parent = std::make_optional<MoveFromParent>(PositionPatch{ row, col, PositionType::NoP }, 
              PositionPatch{ row, col - 1, PositionType::NoP }, 
              PositionPatch{ row, col - 2, PositionType::Peg });
          }
          // can move down
          else if (row < 5 &&
            board[row][col] == PositionType::Peg &&
            board[row + 1][col] == PositionType::Peg &&
            board[row + 2][col] == PositionType::NoP) {
            // Execute right move
            move_from_parent = std::make_optional<MoveFromParent>(PositionPatch{ row, col, PositionType::NoP }, 
              PositionPatch{ row + 1, col, PositionType::NoP },
              PositionPatch{ row + 2, col, PositionType::Peg });
          }
          // can move up
          else if (row > 1 &&
            board[row][col] == PositionType::Peg &&
            board[row - 1][col] == PositionType::Peg &&
            board[row - 2][col] == PositionType::NoP) {
            // Execute right move
            move_from_parent = std::make_optional<MoveFromParent>(PositionPatch{ row, col, PositionType::NoP }, 
              PositionPatch{ row - 1, col, PositionType::NoP }, 
              PositionPatch{ row - 2, col, PositionType::Peg });
          }

          if (move_from_parent) {
            auto peg_position = std::make_shared<Board>();

            //PegPositionContainer patched_board = board;

            peg_position->set_parent(parent_position, move_from_parent.value());

            positions.push_back(peg_position);

            move_from_parent = std::nullopt;
          }
        }
      }
    }

    std::vector<std::shared_ptr<Board>> results;
    auto current_position = positions.back();

    while (current_position->get_parent()) {
      results.push_back(current_position);
      current_position = current_position->get_parent();
    }

    //std::reverse(results.begin(), results.end());

    return results;
  }

  void print_steps(std::vector<std::shared_ptr<Board>> const& steps) {
    for (auto const& board : steps) {
      board->print();
    }
  }

}// namespace ps