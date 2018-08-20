#pragma once
#include <vector>
#include <iostream>
#include <windows.h>
#include <memory>

namespace ps {
  enum class PositionType
  {
    Peg, NoPeg, Invalid
  };

  using Board = std::vector<std::vector<PositionType>>;
  int count = 0;
  struct PegPosition {
    PegPosition() {
      ++count;
    }
    ~PegPosition() {
      --count;
    }

    Board board;
    std::shared_ptr<PegPosition> parent = nullptr;
    int from_row = -1;
    int to_row = -1;
    int from_col = -1;
    int to_col = -1;
  };

  void print_position(PegPosition const& position) {
    SetConsoleOutputCP(65001);
    const auto board = position.board;

    for (auto row = 0u; row < board.size(); ++row) {
      for (auto col = 0u; col < board.front().size(); ++col) {
        if (row == position.from_row && col == position.from_col) {
          std::cout << "*";
        }
        else if (row == position.to_row && col == position.to_col) {
          std::cout << "X";
        }
        else {

          auto const cell = board[row][col];
          switch (cell)
          {
          case PositionType::Peg:
            std::cout << "·";
            break;
          case PositionType::NoPeg:
            std::cout << "o";
            break;
          case PositionType::Invalid:
            std::cout << " ";
            break;
          default:
            break;
          }
        }
      }
      std::cout << "\n";
    }
    std::cout << "\n";
    std::cout << "\n";
  }

  PegPosition get_english_position() {
    PegPosition peg_position;

    auto insert_line = [&peg_position](std::vector<std::pair<PositionType, int>> const& type_positions) {
      std::vector<PositionType> line;

      for (auto& type_position : type_positions) {
        for (int i = 0; i < type_position.second; ++i) {
          line.push_back(type_position.first);
        }
      }

      peg_position.board.emplace_back(std::move(line));
    };

    insert_line({ {PositionType::Invalid, 2},{ PositionType::Peg, 3 }, { PositionType::Invalid, 2 } });
    insert_line({ { PositionType::Invalid, 2 },{ PositionType::Peg, 3 },{ PositionType::Invalid, 2 } });
    insert_line({ { PositionType::Peg, 7 } });
    insert_line({ { PositionType::Peg, 3 },{ PositionType::NoPeg, 1 },{ PositionType::Peg, 3 } });
    insert_line({ { PositionType::Peg, 7 } });
    insert_line({ { PositionType::Invalid, 2 },{ PositionType::Peg, 3 },{ PositionType::Invalid, 2 } });
    insert_line({ { PositionType::Invalid, 2 },{ PositionType::Peg, 3 },{ PositionType::Invalid, 2 } });

    return peg_position;
  }

  int count_pegs(Board const& board) {
    int counter = 0;
    for (auto& row : board) {
      for (auto & col : row) {
        col == PositionType::Peg ? ++counter : 0;
      }
    }
    return counter;
  }

  std::vector<std::shared_ptr<PegPosition>> solve_peg_solitaire(PegPosition const& peg_position) {
    std::vector<std::shared_ptr<PegPosition>> positions;
    auto start_pos = std::make_shared<PegPosition>();
    start_pos->board = peg_position.board;
    positions.push_back(start_pos);

    while (!positions.empty() && count_pegs((*positions.back()).board) != 1) {
      const auto parent_position = positions.back();
      const auto board = parent_position->board;
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
            board[row][col + 2] == PositionType::NoPeg) {
            // Execute right move
            patches.emplace_back(PositionPatch{ row, col, PositionType::NoPeg });
            patches.emplace_back(PositionPatch{ row, col + 1, PositionType::NoPeg });
            patches.emplace_back(PositionPatch{ row, col + 2, PositionType::Peg });
          }
          // can move left
          else if (col > 1 &&
            board[row][col] == PositionType::Peg &&
            board[row][col - 1] == PositionType::Peg &&
            board[row][col - 2] == PositionType::NoPeg) {
            // Execute right move
            patches.emplace_back(PositionPatch{ row, col, PositionType::NoPeg });
            patches.emplace_back(PositionPatch{ row, col - 1, PositionType::NoPeg });
            patches.emplace_back(PositionPatch{ row, col - 2, PositionType::Peg });
          }
          // can move down
          else if (row < 5 &&
            board[row][col] == PositionType::Peg &&
            board[row + 1][col] == PositionType::Peg &&
            board[row + 2][col] == PositionType::NoPeg) {
            // Execute right move
            patches.emplace_back(PositionPatch{ row, col, PositionType::NoPeg });
            patches.emplace_back(PositionPatch{ row + 1, col, PositionType::NoPeg });
            patches.emplace_back(PositionPatch{ row + 2, col, PositionType::Peg });
          }
          // can move up
          else if (row > 1 &&
            board[row][col] == PositionType::Peg &&
            board[row - 1][col] == PositionType::Peg &&
            board[row - 2][col] == PositionType::NoPeg) {            
            // Execute right move
            patches.emplace_back(PositionPatch{ row, col, PositionType::NoPeg });
            patches.emplace_back(PositionPatch{ row - 1, col, PositionType::NoPeg });
            patches.emplace_back(PositionPatch{ row - 2, col, PositionType::Peg });
          }

          if (!patches.empty()) {
            auto peg_position = std::make_shared<PegPosition>();
            // Copy original board
            peg_position->board = board;
            peg_position->parent = parent_position;

            peg_position->from_row = row;
            peg_position->from_col = col;

            // Last patch position it where we move peg
            peg_position->to_row = patches.back().row;
            peg_position->to_col = patches.back().col;

            for (auto&patch : patches) {
              peg_position->board[patch.row][patch.col] = patch.type;        
            }

            positions.push_back(peg_position);
            patches.clear();
          }
        }
      }
    }

    std::vector<std::shared_ptr<PegPosition>> results;
    auto current_position = *positions.rbegin();

    while (current_position->parent) {
      results.push_back(current_position);
      current_position = current_position->parent;
    }

    return results;
  }

  void print_steps(std::vector<std::shared_ptr<PegPosition>> const& steps) {
    for (auto const& position : steps) {
      print_position(*position.get());
    }
  }
}
