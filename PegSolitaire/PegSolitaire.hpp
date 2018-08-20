#pragma once
#include <vector>
#include <iostream>
#include<windows.h>

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
    PegPosition* next = nullptr;
  };

  void print_position(PegPosition const& position) {
    SetConsoleOutputCP(65001);

    for (auto& row : position.board) {
      for (auto& col : row) {
        switch (col)
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
    std::vector<PegPosition> positions;
    positions.push_back(peg_position);

    //while(!positions.empty() && count_pe)

    return {};
  }


  void print_steps(std::vector<PegPosition> const& steps) {

  }
}