#pragma once
#include <array>
#include <string>

class SudokuSolver
{
public:
    using GridArray = std::array<int, 81>;
    using DotsArray = std::array<bool, 72>;
private:
    GridArray given_numbers{};
    GridArray solution{};
    bool solution_found{ false };

    // settings
    bool regbox{ true };

    bool consecutive_dots{ false };
    DotsArray row_consecutive_dots{};
    DotsArray col_consecutive_dots{};

    bool diagonals{ false };

    bool possible(int index, int n, GridArray& grid);
    bool solve_recursive(GridArray& grid, int i);
    int number_of_solutions_recursive(GridArray& grid, int i, int max_number_solutions);
    std::string print_grid_pretty(GridArray& grid);
public:
    SudokuSolver() {};
    SudokuSolver(GridArray& grid,bool diagonals = false);
    SudokuSolver(GridArray& grid, DotsArray& row_dots, DotsArray& col_dots, bool diagonals = false);
    SudokuSolver(GridArray& grid, bool consecutive_dots, DotsArray& row_dots, DotsArray& col_dots, bool diagonals = false);
    bool is_feasible(GridArray& grid);
    bool solve();
    int number_of_solutions(int max_number_solutions = 2);
    std::string print_problem_grid(bool pretty = false);
    std::string print_solution(bool pretty = false);

    void reset_solution();
    bool set_given_numbers(GridArray& grid);
    void set_consecutive_dots(bool on);
    bool get_consecutive_dots() { return consecutive_dots; }
    bool set_row_consecutive_dots(DotsArray& grid);
    DotsArray get_row_consecutive_dots() { return row_consecutive_dots; }
    bool set_col_consecutive_dots(DotsArray& grid);
    DotsArray get_col_consecutive_dots() { return col_consecutive_dots; }
    void set_diagonals(bool on);
    bool get_diagonals() { return diagonals; }
    bool get_solution_found() { return solution_found; }
    int get_solution(int i);
    GridArray get_solution() { return solution; }

    int calls{};
};

