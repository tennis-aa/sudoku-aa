#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
using namespace std::chrono;
#include "SudokuSolver.h"
#ifdef __EMSCRIPTEN__
#include "emscripten_utils.h"
#endif

extern "C"
{
    int solve_sudoku(char grid[82], int consecutive_dots, char row_dots[73], char col_dots[73], int diagonals, char solution[82])
    {
        SudokuSolver::GridArray igrid;
        for (int i = 0; i < 81; ++i)
        {
            int temp = grid[i] - '0';
            if (temp < 0 || temp > 9)
                return false;
            igrid[i] = temp;
        }

        SudokuSolver::DotsArray irow_dots{};
        SudokuSolver::DotsArray icol_dots{};
        if (consecutive_dots)
        {
            for (int i = 0; i < 72; ++i)
            {
                int temp = row_dots[i] - '0';
                if (temp < 0 || temp > 1)
                    return false;
                irow_dots[i] = temp;

                temp = col_dots[i] - '0';
                if (temp < 0 || temp > 1)
                    return false;
                icol_dots[i] = temp;
            }
        }

        SudokuSolver s(igrid, consecutive_dots, irow_dots, icol_dots, diagonals);
        s.solve();
        if (s.get_solution_found())
        {
            for (int i = 0; i < 81; ++i)
            {
                solution[i] = '0' + s.get_solution(i);
                std::cout << solution[i];
            }
            std::cout << "\n";
            return true;
        }

        return false;
    }

    void interactive_mode()
    {
        std::string input;
        SudokuSolver s;
        while (true) // only break with user termination (ctrl + c)
        {
#ifdef __EMSCRIPTEN__
            emscripten_utils_getline(input);
#else
            std::cout << ">> ";
            std::cin;
            getline(std::cin, input);
#endif

            std::stringstream ss(input);
            std::string token;
            if (!(ss >> token))
                continue;
            if (token == "help")
            {
                // TO DO
            }
            else if (token == "grid")
            {
                if (!(ss >> token))
                {
                    std::cout << "grid " << s.print_problem_grid(false) << "\n";
                    continue;
                }
                size_t len = token.length();
                if (len != 81)
                {
                    std::cout << "Invalid length of grid: " << len << " instead of 81\n";
                    continue;
                }
                SudokuSolver::GridArray grid{};
                for (int i = 0; i < len; ++i)
                {
                    int igrid = token[i] - '0';
                    if (igrid < 0 || igrid > 9)
                    {
                        std::cout << "grid can only contain numbers in range 0-9\n";
                        continue;
                    }
                    grid[i] = igrid;
                }
                s.set_given_numbers(grid);
                std::cout << "grid " << token << "\n";
            }
            else if (token == "rowdots")
            {
                if (!(ss >> token))
                {
                    std::cout << "rowdots ";
                    SudokuSolver::DotsArray rowdots = s.get_row_consecutive_dots();
                    for (int i = 0; i < rowdots.size(); ++i)
                    {
                        std::cout << rowdots[i];
                    }
                    std::cout << "\n";
                    continue;
                }
                size_t len = token.length();
                if (len != 72)
                {
                    std::cout << "Invalid length of rowdots: " << len << " instead of 72\n";
                    continue;
                }
                SudokuSolver::DotsArray rowdots{};
                for (int i = 0; i < 72; ++i)
                {
                    int igrid = token[i] - '0';
                    if (igrid < 0 || igrid > 1)
                    {
                        std::cout << "rowdots can only contain 0s and 1s\n";
                        continue;
                    }
                    rowdots[i] = igrid;
                }
                s.set_row_consecutive_dots(rowdots);
                std::cout << "rowdots " << token << "\n";
            }
            else if (token == "coldots")
            {
                if (!(ss >> token))
                {
                    std::cout << "coldots ";
                    SudokuSolver::DotsArray coldots = s.get_col_consecutive_dots();
                    for (int i = 0; i < coldots.size(); ++i)
                    {
                        std::cout << coldots[i];
                    }
                    std::cout << "\n";
                    continue;
                }
                size_t len = token.length();
                if (len != 72)
                {
                    std::cout << "Invalid length of coldots: " << len << " instead of 72\n";
                    continue;
                }
                SudokuSolver::DotsArray coldots{};
                for (int i = 0; i < len; ++i)
                {
                    int igrid = token[i] - '0';
                    if (igrid < 0 || igrid > 1)
                    {
                        std::cout << "coldots can only contain 0s and 1s\n";
                        continue;
                    }
                    coldots[i] = igrid;
                }
                s.set_col_consecutive_dots(coldots);
                std::cout << "coldots " << token << "\n";
            }
            else if (token == "diagonals")
            {
                if (!(ss >> token))
                {
                    std::cout << "diagonals " << (s.get_diagonals() ? "on" : "off") << "\n";
                    continue;
                }
                if (token == "on")
                {
                    s.set_diagonals(true);
                    std::cout << "diagonals on\n";
                }
                else if (token == "off")
                {
                    s.set_diagonals(false);
                    std::cout << "diagonals off\n";
                }
                else
                    std::cout << "Argument for diagonals is either \"on\" or \"off\"";
            }
            else if (token == "dots")
            {
                if (!(ss >> token))
                {
                    std::cout << "dots " << (s.get_consecutive_dots() ? "on" : "off") << "\n";
                    continue;
                }
                if (token == "on")
                {
                    s.set_consecutive_dots(true);
                    std::cout << "dots on\n";
                }
                else if (token == "off")
                {
                    s.set_consecutive_dots(false);
                    std::cout << "dots off\n";
                }
                else
                    std::cout << "Argument for dots is either \"on\" or \"off\"\n";
            }
            else if (token == "solve")
            {
                if (s.solve())
                    std::cout << "Solved\n";
                else
                    std::cout << "No solution was found. The problem has no solution.\n";
            }
            else if (token == "nsolutions")
            {
                if (!(ss >> token))
                    std::cout << "No argument passed to nsolutions. Provide a positive integer as the maximum number of solutions to find to prevent long running times.\n";
                else
                {
                    int max_number = 0;
                    for (int i = 0; i < token.length(); ++i)
                    {
                        int j = token.length() - i - 1;
                        int k = (token[j] - '0');
                        if (k < 0 || k>9)
                        {
                            std::cout << "The argument to nsolutions must be an integer.\n";
                            continue;
                        }
                        max_number += k * pow(10, i);
                    }
                    std::cout << s.number_of_solutions(max_number) << "\n";
                }
            }
            else if (token == "print")
            {
                if (!(ss >> token))
                {
                    std::cout << "No argument passed to print. You need to specify \"problem\" or \"solution\".\n";
                    continue;
                }
                if (token == "problem")
                {
                    if (!(ss >> token) || token == "string")
                        std::cout << "problem " << s.print_problem_grid(false) << "\n";
                    else if (token == "pretty")
                        std::cout << s.print_problem_grid(true);
                    else
                        std::cout << token << " form of printing does not exist. Use either \"string\" or \"pretty\" (Blank defaults to \"string\").\n";

                }
                else if (token == "solution")
                {
                    if (!s.get_solution_found())
                    {
                        std::cout << "A solution has not been computed. Run solve to get a solution.\n";
                        continue;
                    }
                    if (!(ss >> token) || token == "string")
                        std::cout << "solution " << s.print_solution(false) << "\n";
                    else if (token == "pretty")
                        std::cout << s.print_solution(true);
                    else
                        std::cout << token << " form of printing does not exist. Use either \"string\" or \"pretty\" (Blank defaults to \"string\")\n";
                }
                else
                {
                    std::cout << "Argument to print is either \"problem\" or \"solution\".\n";
                }
            }
            else
            {
                std::cout << "invalid command: " << token << "\n";
            }
        }
    }
}

int main(int argc, char** argv)
{
    //test arguments
    // --grid 000009000000000000000000000000002000000004000000000060000000000000800000000000000 --dots --rowdots 000000000000000000000010100000101110000000001000000100000000100000000000 --coldots 101001000000000001000001010001001000001000000100100010001000100001001000

    // defaults
    // --grid 000009000000000000000000000000002000000004000000000060000000000000800000000000000
    SudokuSolver::GridArray grid =
    { 0,0,0,0,0,9,0,0,0,
      0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0,
      0,0,0,0,0,2,0,0,0,
      0,0,0,0,0,4,0,0,0,
      0,0,0,0,0,0,0,6,0,
      0,0,0,0,0,0,0,0,0,
      0,0,0,8,0,0,0,0,0,
      0,0,0,0,0,0,0,0,0
    };

    // --rowdots 000000000000000000000010100000101110000000001000000100000000100000000000
    SudokuSolver::DotsArray row_dots =
    { 0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,1,0,
      1,0,0,0,0,0,1,0,
      1,1,1,0,0,0,0,0,
      0,0,0,0,1,0,0,0,
      0,0,0,1,0,0,0,0,
      0,0,0,0,1,0,0,0,
      0,0,0,0,0,0,0,0
    };


    // --coldots 101001000000000001000001010001001000001000000100100010001000100001001000
    SudokuSolver::DotsArray col_dots =
    { 1,0,1,0,0,1,0,0,
      0,0,0,0,0,0,0,0,
      0,1,0,0,0,0,0,1,
      0,1,0,0,0,1,0,0,
      1,0,0,0,0,0,1,0,
      0,0,0,0,0,1,0,0,
      1,0,0,0,1,0,0,0,
      1,0,0,0,1,0,0,0,
      0,1,0,0,1,0,0,0
    };

    // --grid 600000000000000750000018000100020090000000600047090000080309005420800000000000003
    SudokuSolver::GridArray grid2 =
    { 6,0,0,0,0,0,0,0,0,
      0,0,0,0,0,0,7,5,0,
      0,0,0,0,1,8,0,0,0,
      1,0,0,0,2,0,0,9,0,
      0,0,0,0,0,0,6,0,0,
      0,4,7,0,9,0,0,0,0,
      0,8,0,3,0,9,0,0,5,
      4,2,0,8,0,0,0,0,0,
      0,0,0,0,0,0,0,0,3
    };

    bool diagonals = false;
    bool consecutive_dots = false;

    int iarg = 1;
    while (iarg < argc)
    {
        if (std::strcmp(argv[iarg], "--interactive") == 0 || std::strcmp(argv[iarg], "-i") == 0)
        {
            std::cout << "Entering SudokuSolver interactive mode...\n";
            interactive_mode();
            return 0;
        }
        if (std::strcmp(argv[iarg], "--grid") == 0 || std::strcmp(argv[iarg], "-g") == 0)
        {
            size_t len = std::strlen(argv[iarg + 1]);
            if (len != 81)
            {
                std::cout << "--grid/-g: the argument is of length " << len << " and must be of length 81\n";
                return 1;
            }
            for (int i = 0; i < len; ++i)
            {
                int igrid = argv[iarg + 1][i] - '0';
                if (igrid < 0 || igrid > 9)
                {
                    std::cout << "--grid/-g: the argument can only contain numbers in range 0-9\n";
                    return 1;
                }
                grid[i] = igrid;
            }
            iarg += 2;
        }
        else if (std::strcmp(argv[iarg], "--rowdots") == 0 || std::strcmp(argv[iarg], "-rd") == 0)
        {
            size_t len = std::strlen(argv[iarg + 1]);
            if (len != 72)
            {
                std::cout << "--rowdots/-rd: the argument must be a string of digits of length 72\n";
                return 1;
            }
            for (int i = 0; i < len; ++i)
            {
                int igrid = argv[iarg + 1][i] - '0';
                if (igrid < 0 || igrid > 9)
                {
                    std::cout << "--rowdots/-rd: the argument can only contain numbers in range 0-9\n";
                    return 1;
                }
                row_dots[i] = igrid;
            }
            iarg += 2;
        }
        else if (std::strcmp(argv[iarg], "--coldots") == 0 || std::strcmp(argv[iarg], "-cd") == 0)
        {
            size_t len = std::strlen(argv[iarg + 1]);
            if (len != 72)
            {
                std::cout << "--coldots/-cd: the argument must be a string of digits of length 72\n";
                return 1;
            }
            for (int i = 0; i < len; ++i)
            {
                int igrid = argv[iarg + 1][i] - '0';
                if (igrid < 0 || igrid > 9)
                {
                    std::cout << "--coldots/-cd: the argument can only contain numbers in range 0-9\n";
                    return 1;
                }
                col_dots[i] = igrid;
            }
            iarg += 2;
        }
        else if (std::strcmp(argv[iarg], "--dots") == 0)
        {
            consecutive_dots = true;
            ++iarg;
        }
        else if (std::strcmp(argv[iarg], "--diagonals") == 0)
        {
            diagonals = true;
            ++iarg;
        }
        else
        {
            std::cout << argv[iarg] << " is an unknown argument.\n";
            ++iarg;
        }
    }

    SudokuSolver s(grid, consecutive_dots, row_dots, col_dots, diagonals);
    //SudokuSolver s(grid2,true);

    s.print_problem_grid(true);

    auto start = high_resolution_clock::now();
    s.solve();
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    std::cout << "duration: " << duration.count() << " milliseconds" << "\n";
    std::cout << "\n" << "calls: " << s.calls << "\n";

    s.print_solution(true);

    start = high_resolution_clock::now();
    int nsolutions = s.number_of_solutions(100);
    stop = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(stop - start);

    std::cout << "\n";
    std::cout << "Number of solutions: " << nsolutions << "\n";
    std::cout << "duration: " << duration.count() << " milliseconds" << "\n";
    std::cout << "\n" << "calls: " << s.calls << "\n";

    return 0;
}
