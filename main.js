import { Sudoku_aa, Sudoku_aa_solution } from "./src/sudoku_aa.js";

let sudoku_aa_container = document.getElementById("sudoku-aa");
let sudoku_aa = new Sudoku_aa(sudoku_aa_container);

let sudoku_aa_solution_container = document.getElementById("sudoku-aa-solution");
let sudoku_aa_solution = new Sudoku_aa_solution(sudoku_aa_solution_container,sudoku_aa);

window.restart = function() {
  sudoku_aa.clear();
  sudoku_aa_solution.clear();
};

window.solve = function() {
  sudoku_aa.solve();
};

window.consecutive = function() {
  sudoku_aa.activateDots();
};

window.diagonals = function() {
  sudoku_aa.activateDiagonals();
}