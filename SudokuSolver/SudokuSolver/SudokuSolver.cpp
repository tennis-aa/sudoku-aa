#include <iostream>
#include <sstream>
#include "SudokuSolver.h"

SudokuSolver::SudokuSolver(GridArray& grid, bool diagonals)
    :diagonals{ diagonals }
{
    for (int i = 0; i < 81; ++i)
        given_numbers[i] = grid[i];
}

SudokuSolver::SudokuSolver(GridArray& grid, DotsArray& row_dots, DotsArray& col_dots, bool diagonals)
    :SudokuSolver(grid, diagonals)
{
    consecutive_dots = true;
    for (int i = 0; i < 72; ++i)
    {
        row_consecutive_dots[i] = row_dots[i];
        col_consecutive_dots[i] = col_dots[i];
    }
}

SudokuSolver::SudokuSolver(GridArray& grid, bool consecutive_dots, DotsArray& row_dots, DotsArray& col_dots, bool diagonals)
    : SudokuSolver(grid, diagonals)
{
    this->consecutive_dots = consecutive_dots;
    if (consecutive_dots)
    {
        for (int i = 0; i < 72; ++i)
        {
            row_consecutive_dots[i] = row_dots[i];
            col_consecutive_dots[i] = col_dots[i];
        }
    }
}

// helper to find difference of 1
bool oneaway(int x, int y)
{
    if (x - y == 1 || y - x == 1)
        return true;
    return false;
}

bool SudokuSolver::possible(int index, int n, GridArray& grid)
{
    if (index >= 81 || index < 0)
    {
        std::cout << "WARNING SudokuSolver::possible: index=" << index << " is not in valid range 0-80";
        return false;
    }
    if (n < 1 || n>9)
    {
        std::cout << "WARNING SudokuSolver::possible: n=" << n << " is not in valid range 0-9";
        return false;
    }
    int y = index / 9;
    int x = index % 9;
    // Check row
    for (int i = 0; i < 9; ++i)
    {
        if (grid[y * 9 + i] == n)
            return false;
    }
    // Check column
    for (int i = 0; i < 9; ++i)
    {
        if (grid[9 * i + x] == n)
            return false;
    }
    // Check regular boxes
    if (regbox)
    {
        int x0 = (x / 3) * 3;
        int y0 = (y / 3) * 3;
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                if (grid[9 * (y0 + i) + x0 + j] == n)
                    return false;
    }

    // Check diagonals
    if (diagonals) {
        // First digonal
        if (x == y) {
            for (int i = 0; i < 9; ++i) {
                if (grid[9 * i + i] == n)
                    return false;
            }
        }
        // Second diagonal
        if (x == 8 - y) {
            for (int i = 0; i < 9; ++i) {
                if (grid[9 * (8 - i) + i] == n)
                    return false;
            }
        }
    }

    // consecutive dots
    if (consecutive_dots)
    {
        if (x < 8 && grid[9 * y + x + 1] != 0) {
            if (row_consecutive_dots[8 * y + x]) {
                if (!oneaway(grid[9 * y + x + 1], n)) {
                    return false;
                }
            }
            else {
                if (oneaway(grid[9 * y + x + 1], n)) {
                    return false;
                }
            }
        }
        // look left
        if (x > 0 && grid[9 * y + x - 1] != 0) {
            if (row_consecutive_dots[8 * y + x - 1]) {
                if (!oneaway(grid[9 * y + x - 1], n)) {
                    return false;
                }
            }
            else {
                if (oneaway(grid[9 * y + x - 1], n)) {
                    return false;
                }
            }
        }
        // look down
        if (y < 8 && grid[9 * (y + 1) + x] != 0) {
            if (col_consecutive_dots[8 * x + y]) {
                if (!oneaway(grid[9 * (y + 1) + x], n)) {
                    return false;
                }
            }
            else {
                if (oneaway(grid[9 * (y + 1) + x], n)) {
                    return false;
                }
            }
        }
        if (y > 0 && grid[9 * (y - 1) + x] != 0) {
            if (col_consecutive_dots[8 * x + y - 1]) {
                if (!oneaway(grid[9 * (y - 1) + x], n)) {
                    return false;
                }
            }
            else {
                if (oneaway(grid[9 * (y - 1) + x], n)) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool SudokuSolver::solve_recursive(GridArray& grid, int i)
{
    ++calls;
    if (i < 81) {
        if (grid[i] == 0) {
            for (int n = 1; n < 10; ++n) {
                if (possible(i, n, grid)) {
                    grid[i] = n;
                    if (solve_recursive(grid, i + 1))
                        return true;
                    else
                        grid[i] = 0;
                }
            }
            return false;
        }
        else
        {
            return solve_recursive(grid, i + 1);
        }
    }
    return true;
}

bool SudokuSolver::is_feasible(GridArray& grid)
{
    for (int i = 0; i < 81; ++i)
    {
        if (grid[i] != 0)
        {
            int temp = grid[i];
            grid[i] = 0;
            if (!possible(i, temp, grid))
            {
                std::cout << "The problem is infeasible because the value " << temp << " is not allowed in index " << i << "\n";
                grid[i] = temp;
                return false;
            }
            else
            {
                grid[i] = temp;
            }
        }
    }
    return true;
}

bool SudokuSolver::solve()
{
    if (!is_feasible(given_numbers))
    {
        return false;
    }
    reset_solution();
    calls = 0;
    if (solve_recursive(solution, 0))
    {
        solution_found = true;
        return true;
    }
    return false;
}

int SudokuSolver::number_of_solutions_recursive(GridArray& grid, int i, int max_number_solutions)
{
    ++calls;
    if (i < 81) {
        if (grid[i] == 0) {
            int count = 0;
            for (int n = 1; n < 10; ++n) {
                if (possible(i, n, grid)) {
                    grid[i] = n;
                    count += number_of_solutions_recursive(grid, i + 1, max_number_solutions);
                    if (count >= max_number_solutions) // limit number of solutions to prevent long running times
                        return count;
                }
            }
            grid[i] = 0;
            return count;
        }
        else
        {
            return number_of_solutions_recursive(grid, i + 1, max_number_solutions);
        }
    }
    return 1;
}

int SudokuSolver::number_of_solutions(int max_number_solutions)
{
    GridArray grid;
    for (int i = 0; i < 81; ++i)
    {
        grid[i] = given_numbers[i];
    }
    calls = 0;
    return number_of_solutions_recursive(grid, 0, max_number_solutions);
}

std::string SudokuSolver::print_grid_pretty(GridArray& grid)
{
    std::stringstream out{};
    char col_sep = '|';
    char row_sep = '-';
    char row_cons = 'o'; //232
    char col_cons = 'o';
    for (int i = 0; i < 9; ++i)
    {
        out << col_sep << ' ';
        for (int j = 0; j < 8; ++j)
        {
            out << grid[9 * i + j] << ' ';
            if (consecutive_dots && row_consecutive_dots[8 * i + j])
                out << row_cons;
            else
                out << col_sep;
            out << ' ';
        }
        out << grid[9 * i + 8] << ' ' << col_sep << '\n';
        if (consecutive_dots && i < 8)
        {
            out << col_sep << row_sep;
            for (int j = 0; j < 8; ++j)
            {
                out << (col_consecutive_dots[8 * j + i] ? col_cons : row_sep) << row_sep << col_sep << row_sep;
            }
            out << (col_consecutive_dots[8 * 8 + i] ? col_cons : row_sep) << row_sep << col_sep << '\n';
        }
    }
    return out.str();
}

// TO DO: show diagonals on pretty and normal printing
//        show rowdots and coldots in normal printing
//        return a string instead of printing to cout
std::string SudokuSolver::print_problem_grid(bool pretty)
{
    if (pretty)
        return print_grid_pretty(given_numbers);
    else
    {
        std::stringstream out{};
        for (int i = 0; i < 81; ++i)
            out << given_numbers[i];
        return out.str();
    }
}

std::string SudokuSolver::print_solution(bool pretty)
{
    if (!solution_found)
        return "";
    if (pretty)
        return print_grid_pretty(solution);
    else
    {
        std::stringstream out{};
        for (int i = 0; i < 81; ++i)
            out << solution[i];
        return out.str();
    }
}

void SudokuSolver::reset_solution()
{
    solution_found = false;
    for (int i = 0; i < 81; ++i)
    {
        solution[i] = given_numbers[i];
    }
}

bool SudokuSolver::set_given_numbers(GridArray& grid)
{
    for (int i = 0; i < 81; ++i)
    {
        if (grid[i] < 0 || grid[i] > 9)
        {
            std::cout << "The given numbers are not in range 0-9\n";
            return false;
        }
    }
    for (int i = 0; i < 81; ++i)
    {
        given_numbers[i] = grid[i];
    }
    reset_solution();
    return true;
}

void SudokuSolver::set_consecutive_dots(bool on)
{
    if (consecutive_dots != on)
    {
        consecutive_dots = on;
        reset_solution();
    }
}

bool SudokuSolver::set_row_consecutive_dots(DotsArray& grid)
{
    for (int i = 0; i < 72; ++i)
    {
        row_consecutive_dots[i] = grid[i];
    }
    if (consecutive_dots)
        reset_solution();
    return true;
}

bool SudokuSolver::set_col_consecutive_dots(DotsArray& grid)
{
    for (int i = 0; i < 72; ++i)
    {
        col_consecutive_dots[i] = grid[i];
    }
    if (consecutive_dots)
        reset_solution();
    return true;
}

void SudokuSolver::set_diagonals(bool on)
{
    if (diagonals != on)
    {
        diagonals = on;
        reset_solution();
    }
}

int SudokuSolver::get_solution(int i)
{
    if (i < 0 || i > 81)
        return 0;
    return solution[i];
}
