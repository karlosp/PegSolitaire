#pragma once
#include <vector>
#include <iostream>
#include <windows.h>
#include <memory>
#include <deque>
#include <optional>

namespace ps {
  enum PositionType
  {
    Inv /*Invalid*/, NoP /*No Peg*/, Peg
  };

  using PegPositionContainer = std::vector<std::vector<PositionType>>;

  class Board {
  public:
    void set_positions(PegPositionContainer && new_positions, std::optional<int> peg_count = std::nullopt) {
      positions = std::move(new_positions);

      if (peg_count) {
        peg_counter = peg_count.value();
      }
      else {
        peg_counter = 0;
        // Count pegs
        for (auto& row : positions) {
          for (auto & col : row) {
            if (col == PositionType::Peg) { ++peg_counter; }
          }
        }
      }
    }

    PegPositionContainer const& get_positions() const {
      return positions;
    }

    int peg_count() const {
      return peg_counter;
    }

    std::shared_ptr<Board> parent = nullptr;

    int from_row = -1;
    int to_row = -1;
    int from_col = -1;
    int to_col = -1;
  private:
    PegPositionContainer positions;
    int peg_counter = 0;
  };

  void print(std::shared_ptr<Board> const& board) {
    SetConsoleOutputCP(65001);
    std::cout << "Peg count: " << board->peg_count() << "\n";

    auto& positions = board->get_positions();

    for (auto row = 0u; row < positions.size(); ++row) {
      for (auto col = 0u; col < positions.front().size(); ++col) {
        if (row == board->from_row && col == board->from_col) {
          std::cout << "*";
        }
        else if (row == board->to_row && col == board->to_col) {
          std::cout << "X";
        }
        else {
          auto const cell = positions[row][col];
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
    std::cout << "\n";
  }

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

      struct PositionPatch {
        int row = 0;
        int col = 0;
        PositionType type;
      };

      std::vector<PositionPatch> patches;
      // generate all possible positions
      for (auto row = 0; row < (int)board.size(); ++row) {
        for (auto col = 0; col < (int)board.front().size(); ++col) {

          // can move rigth
          if (col < 5 &&
            board[row][col] == PositionType::Peg &&
            board[row][col + 1] == PositionType::Peg &&
            board[row][col + 2] == PositionType::NoP) {
            // Execute right move
            patches.emplace_back(PositionPatch{ row, col, PositionType::NoP });
            patches.emplace_back(PositionPatch{ row, col + 1, PositionType::NoP });
            patches.emplace_back(PositionPatch{ row, col + 2, PositionType::Peg });
          }
          // can move left
          else if (col > 1 &&
            board[row][col] == PositionType::Peg &&
            board[row][col - 1] == PositionType::Peg &&
            board[row][col - 2] == PositionType::NoP) {
            // Execute right move
            patches.emplace_back(PositionPatch{ row, col, PositionType::NoP });
            patches.emplace_back(PositionPatch{ row, col - 1, PositionType::NoP });
            patches.emplace_back(PositionPatch{ row, col - 2, PositionType::Peg });
          }
          // can move down
          else if (row < 5 &&
            board[row][col] == PositionType::Peg &&
            board[row + 1][col] == PositionType::Peg &&
            board[row + 2][col] == PositionType::NoP) {
            // Execute right move
            patches.emplace_back(PositionPatch{ row, col, PositionType::NoP });
            patches.emplace_back(PositionPatch{ row + 1, col, PositionType::NoP });
            patches.emplace_back(PositionPatch{ row + 2, col, PositionType::Peg });
          }
          // can move up
          else if (row > 1 &&
            board[row][col] == PositionType::Peg &&
            board[row - 1][col] == PositionType::Peg &&
            board[row - 2][col] == PositionType::NoP) {
            // Execute right move
            patches.emplace_back(PositionPatch{ row, col, PositionType::NoP });
            patches.emplace_back(PositionPatch{ row - 1, col, PositionType::NoP });
            patches.emplace_back(PositionPatch{ row - 2, col, PositionType::Peg });
          }

          if (!patches.empty()) {
            auto peg_position = std::make_shared<Board>();

            PegPositionContainer patched_board = board;

            peg_position->parent = parent_position;

            // Starting peg position
            peg_position->from_row = row;
            peg_position->from_col = col;

            // Last patch position it where we move peg
            peg_position->to_row = patches.back().row;
            peg_position->to_col = patches.back().col;

            for (auto&patch : patches) {
              patched_board[patch.row][patch.col] = patch.type;
            }

            patched_peg_count.emplace(parent_position->peg_count() - 1);
            peg_position->set_positions(std::move(patched_board), patched_peg_count);
            positions.push_back(peg_position);
            patches.clear();
          }
        }
      }
    }

    std::vector<std::shared_ptr<Board>> results;
    auto current_position = *positions.rbegin();

    while (current_position->parent) {
      results.push_back(current_position);
      current_position = current_position->parent;
    }

    return results;
  }

  void print_steps(std::vector<std::shared_ptr<Board>> const& steps) {
    for (auto const& position : steps) {
      print(position);
    }
  }
}