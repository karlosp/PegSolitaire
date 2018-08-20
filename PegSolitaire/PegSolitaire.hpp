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

  struct PegPosition {
    //public:
    //  std::vector<PositionType> const& get_board() {
    //    return board;
    //  }
    //private:
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

  std::vector<PegPosition> solve_peg_solitaire(PegPosition const& peg_position) {
    std::vector<std::shared_ptr<PegPosition>> positions;
    auto start_pos = std::make_shared<PegPosition>();
    start_pos->board = peg_position.board;
    positions.push_back(start_pos);

    const int most_right_col = positions.back()->board.size() - 3;
    const int most_left_col = 2;
    const int most_bottom_row = positions.back()->board.front().size() - 3;
    const int most_top_row = 2;

    while (!positions.empty() && count_pegs((*positions.back()).board) != 1) {
      const auto parent_position = positions.back();
      const auto board = parent_position->board;
      positions.pop_back();

      //std::cout << "Parent:\n";
      //print_position(*parent_position);

      // generate all possible positions
      for (auto row = 0u; row < board.size(); ++row) {
        for (auto col = 0u; col < board.front().size(); ++col) {

          // can move rigth
          if (col < 5 &&
            board[row][col] == PositionType::Peg &&
            board[row][col + 1] == PositionType::Peg &&
            board[row][col + 2] == PositionType::NoPeg) {
            auto peg_position = std::make_shared<PegPosition>();
            // Copy original board
            peg_position->board = board;
            // Execute right move
            peg_position->board[row][col] = PositionType::NoPeg;
            peg_position->board[row][col + 1] = PositionType::NoPeg;
            peg_position->board[row][col + 2] = PositionType::Peg;

            peg_position->parent = parent_position;
            peg_position->from_row = row;
            peg_position->from_col = col;

            peg_position->to_row = row;
            peg_position->to_col = col + 2;

            //std::cout << "Adding to queue move right:\n";
            //print_position(*peg_position);
            positions.push_back(peg_position);
          }
          // can move left
          else if (col > 1 &&
            board[row][col] == PositionType::Peg &&
            board[row][col - 1] == PositionType::Peg &&
            board[row][col - 2] == PositionType::NoPeg) {
            auto peg_position = std::make_shared<PegPosition>();
            // Copy original board
            peg_position->board = board;
            // Execute right move
            peg_position->board[row][col] = PositionType::NoPeg;
            peg_position->board[row][col - 1] = PositionType::NoPeg;
            peg_position->board[row][col - 2] = PositionType::Peg;

            peg_position->parent = parent_position;
            peg_position->from_row = row;
            peg_position->from_col = col;

            peg_position->to_row = row;
            peg_position->to_col = col - 2;
            positions.push_back(peg_position);
          }
          // can move down
          else if (row < 5 &&
            board[row][col] == PositionType::Peg &&
            board[row + 1][col] == PositionType::Peg &&
            board[row + 2][col] == PositionType::NoPeg) {
            auto peg_position = std::make_shared<PegPosition>();
            // Copy original board
            peg_position->board = board;
            // Execute right move
            peg_position->board[row][col] = PositionType::NoPeg;
            peg_position->board[row + 1][col] = PositionType::NoPeg;
            peg_position->board[row + 2][col] = PositionType::Peg;

            peg_position->parent = parent_position;
            peg_position->from_row = row;
            peg_position->from_col = col;

            peg_position->to_row = row + 2;
            peg_position->to_col = col;
            positions.push_back(peg_position);
          }
          // can move up
          else if (row > 1 &&
            board[row][col] == PositionType::Peg &&
            board[row - 1][col] == PositionType::Peg &&
            board[row - 2][col] == PositionType::NoPeg) {
            auto peg_position = std::make_shared<PegPosition>();
            // Copy original board
            peg_position->board = board;
            // Execute right move
            peg_position->board[row][col] = PositionType::NoPeg;
            peg_position->board[row - 1][col] = PositionType::NoPeg;
            peg_position->board[row - 2][col] = PositionType::Peg;

            peg_position->parent = parent_position;
            peg_position->from_row = row;
            peg_position->from_col = col;

            peg_position->to_row = row - 2;
            peg_position->to_col = col;
            positions.push_back(peg_position);
          }
        }
      }
    }

    std::vector<PegPosition> results;
    auto current_position = *positions.rbegin();

    while (current_position->parent) {
      results.push_back(*current_position);
      current_position = current_position->parent;
    }

    return results;
  }


  void print_steps(std::vector<PegPosition> const& steps) {
    for (auto const& position : steps) {
      print_position(position);
    }
  }
}