export class Sudoku_aa {
  constructor (main_div) {
    this.solver = new Worker("./SudokuSolver.js");
    this.solver.onmessage = this.solverOnmessage();

    this.grid = new Array(81).fill(0);

    this.dispatcher = document.createElement("div");

    this.container = document.createElement("div");
    this.container.style.position = "relative";
    this.container.style.aspectRatio = "1/1";
    this.container.style.height = "fit-content";
    this.container.style.userSelect = "none";

    this.boxoutline = document.createElement("div");
    this.boxoutline.style.position = "absolute";
    this.boxoutline.style.left = 0;
    this.boxoutline.style.top = 0;
    this.boxoutline.style.height = "100%";
    this.boxoutline.style.width = "100%";
    this.boxoutline.style.display = "grid";
    this.boxoutline.style.gridTemplateColumns = "repeat(3,33.33333%)";
    this.boxoutline.style.gridTemplateRows = "repeat(3,33.33333%)";
    this.container.appendChild(this.boxoutline);

    for (let i = 0; i < 9; i++) {
      let div = document.createElement("div");
      div.style.border = "2px solid black";
      this.boxoutline.appendChild(div);
    }

    this.sudokuboard = document.createElement("div");
    this.sudokuboard.style.position = "absolute";
    this.sudokuboard.style.left = 0;
    this.sudokuboard.style.top = 0;
    this.sudokuboard.style.height = "100%";
    this.sudokuboard.style.width = "100%";
    this.sudokuboard.style.display = "grid";
    this.sudokuboard.style.gridTemplateColumns = "repeat(9,11.11111%)";
    this.sudokuboard.style.gridTemplateRows = "repeat(9,11.11111%)";
    this.container.appendChild(this.sudokuboard);

    for (let i = 0; i < 81; i++) {
      let div = document.createElement("div");
      div.className = "sudoku-aa-square";
      div.style.position = "relative";
      div.style.display = "flex";
      div.style.alignItems = "center";
      div.style.justifyContent = "center";
      div.style.fontFamily = "sans-serif";
      div.style.fontSize = "40px";
      div.style.border = "1px solid black";
      div.tabIndex = i+1;
      div.style.outline = "none"; // prevent blue outline on focus
      div.onfocus = this.focusSquare();
      div.onblur = this.blurSquare();
      div.onkeydown = this.keydownCommands();
      this.sudokuboard.appendChild(div);
    }

    // initialize consecutive rows and columns
    this.consecutive_flag = true;
    this.consecutive_rows = new Array(9*8).fill(false);
    this.consecutive_cols = new Array(9*8).fill(false);

    this.rowDots = [];
    this.colDots = [];
    for (let i=0; i<9; ++i) {
      for (let j=0; j<8; ++j) {
        let but_row = this.createDots(100/9*(j+1), 100/18+100*i/9);
        but_row.onclick = this.activateRowDot(i,j);
        this.rowDots.push(but_row);
        this.container.appendChild(but_row);
      }
    }
    for (let i=0; i<9; ++i) {
      for (let j=0; j<8; ++j) {
        let but_col = this.createDots(100/18+100*i/9,100/9*(j+1));
        but_col.onclick = this.activateColDot(i,j);
        this.colDots.push(but_col);
        this.container.appendChild(but_col);
      }
    }
    this.activateDots(); // deactivate dots

    this.svgcontainer = this.createSVG();
    this.container.appendChild(this.svgcontainer);
    this.diagonals = false;
    this.svgdiagonals = [];

    main_div.append(this.container);
  }

  focusSquare() {
    return function(event) {
      let div = event.currentTarget;
      div.style.border = "2px solid blue";
    };
  }

  blurSquare() {
    return function(event) {
      let div = event.currentTarget;
      div.style.border = "1px solid black";
    }
  }

  keydownCommands() {
    let that = this;
    return function(event) {
      let div = event.currentTarget;
      let index = Array.from(that.sudokuboard.children).indexOf(div);
      let val = event.key;
      if ("123456789".includes(val)) {
        that.grid[index] = parseInt(val);
        div.textContent = val;
      }
      else if (val == "Delete" || val == "Backspace") {
        that.grid[index] = 0;
        div.textContent = "";
      } 
    };
  }

  createSVG() {
    let svgns = "http://www.w3.org/2000/svg";
    let svg = document.createElementNS(svgns,"svg");
    svg.style.position = "absolute";
    svg.style.top = "0";
    svg.style.left = "0";
    svg.style.opacity = "0.6";
    svg.style.pointerEvents = "none";
    svg.style.zIndex = "-1";
    svg.setAttribute("height", "100%");
    svg.setAttribute("apect-ratio", "1/1");
    svg.setAttribute("viewBox", "0 0 9 9");
    return svg;
  }

  activateDiagonals() {
    let svgns = "http://www.w3.org/2000/svg";
    if (this.diagonals) {
      this.diagonals = false;
      this.svgdiagonals[0].remove();
      this.svgdiagonals[1].remove();
    }
    else{
      this.diagonals = true;
      if (this.svgdiagonals.length == 2) {
        this.svgcontainer.appendChild(this.svgdiagonals[0]);
        this.svgcontainer.appendChild(this.svgdiagonals[1]);
      }
      else {
        let path = document.createElementNS(svgns, "line");
        path.setAttribute("x1", 0);
        path.setAttribute("y1", 0);
        path.setAttribute("x2", 9);
        path.setAttribute("y2", 9);
        this.svgdiagonals.push(path);
        path = document.createElementNS(svgns, "line");
        path.setAttribute("x1", 0);
        path.setAttribute("y1", 9);
        path.setAttribute("x2", 9);
        path.setAttribute("y2", 0);
        this.svgdiagonals.push(path);
        for (let i=0; i<this.svgdiagonals.length; ++i) {
          let path = this.svgdiagonals[i];
          path.style.stroke = "red";
          path.style.strokeWidth = "0.1";
          path.style.fill = "none";
          this.svgcontainer.appendChild(path);
        }
      }
    }
  }

  createDots(x,y) {
    let dotdim = 2;
    let but = document.createElement("div");
    but.style.backgroundColor = "white";
    but.style.width = dotdim + "%";
    but.style.height = dotdim + "%";
    but.style.position = "absolute";
    but.style.left = (x - dotdim/2) + "%";
    but.style.top = (y - dotdim/2) + "%";
    but.style.borderRadius = 100 + "%";
    but.style.border = "1px solid black";
    return but;
  }

  activateRowDot(i,j) {
    let that = this;
    return function (event) {
      if (that.consecutive_rows[8*i + j]) {
        that.consecutive_rows[8*i + j] = false;
        event.currentTarget.style.boxShadow = "none";
      }
      else {
        that.consecutive_rows[8*i +j] = true;
        event.currentTarget.style.boxShadow = "inset 0 0 0 1000px red";
      }
    };
  }

  activateColDot(i,j) {
    let that = this;
    return function (event) {
      if (that.consecutive_cols[8*i + j]) {
        that.consecutive_cols[8*i + j] = false;
        event.currentTarget.style.boxShadow = "none";
      }
      else {
        that.consecutive_cols[8*i +j] = true;
        event.currentTarget.style.boxShadow = "inset 0 0 0 1000px red";
      }
    };
  }

  activateDots() {
    this.consecutive_flag = !this.consecutive_flag;
    for (let i=0; i<9*8; ++i) {
      this.rowDots[i].style.visibility = this.consecutive_flag ? "visible" : "hidden";
      this.colDots[i].style.visibility = this.consecutive_flag ? "visible" : "hidden";
    }
  }

  solve() {
    // if (!confirm("Are you sure you want to see the solution?")) {
    //   return;
    // }
    this.solver.postMessage("grid " + this.grid.join(""));
    this.solver.postMessage("diagonals " + (this.diagonals ? "on" : "off"));
    this.solver.postMessage("dots " + (this.consecutive_flag ? "on" : "off"));
    if (this.consecutive_flag) {
      let rowdots = "";
      let coldots = "";
      for (let i=0; i<this.consecutive_rows.length; ++i) {
        rowdots += this.consecutive_rows[i] ? 1 : 0;
        coldots += this.consecutive_cols[i] ? 1 : 0;
      }
      this.solver.postMessage("rowdots " + rowdots);
      this.solver.postMessage("coldots " + coldots);
    }
    this.solver.postMessage("solve");
    this.solver.postMessage("print solution");
    this.dispatcher.dispatchEvent(new Event("sudoku-aa-searching"));
  }

  solverOnmessage() {
    let that = this;
    return function(event) {
      let line = event.data;
      if (line.startsWith("solution")) {
        let solution = line.slice(9);
        let event = new CustomEvent("sudoku-aa-solution",{ detail:{ grid: that.grid, solution: solution } });
        that.dispatcher.dispatchEvent(event);
      }
      else if (line.startsWith("No solution was found")) {
        alert("No solution was found");
      }
    }
  }

  clear() {
    for (let i=0; i<81; ++i) {
      this.sudokuboard.children[i].textContent = "";
      this.grid[i] = 0;
      for (let i=0; i<8*9;++i) {
        this.consecutive_rows[i] = false;
        this.consecutive_cols[i] = false;
        this.rowDots[i].style.backgroundColor = "white";
        this.colDots[i].style.backgroundColor = "white";
      }
    }
  }
}





export class Sudoku_aa_solution {
  constructor(main_div,sudoku_aa) {
    this.sudoku_aa = sudoku_aa;

    this.container = document.createElement("div");
    this.container.style.position = "relative";
    this.container.style.aspectRatio = "1/1";
    this.container.style.height = "fit-content";
    this.container.style.userSelect = "none";

    this.boxoutline = document.createElement("div");
    this.boxoutline.style.position = "absolute";
    this.boxoutline.style.left = 0;
    this.boxoutline.style.top = 0;
    this.boxoutline.style.height = "100%";
    this.boxoutline.style.width = "100%";
    this.boxoutline.style.display = "grid";
    this.boxoutline.style.gridTemplateColumns = "repeat(3,33.33333%)";
    this.boxoutline.style.gridTemplateRows = "repeat(3,33.33333%)";

    for (let i = 0; i < 9; i++) {
      let div = document.createElement("div");
      div.style.border = "2px solid black";
      this.boxoutline.appendChild(div);
    }

    this.sudokuboard = document.createElement("div");
    this.sudokuboard.style.position = "absolute";
    this.sudokuboard.style.left = 0;
    this.sudokuboard.style.top = 0;
    this.sudokuboard.style.height = "100%";
    this.sudokuboard.style.width = "100%";
    this.sudokuboard.style.display = "grid";
    this.sudokuboard.style.gridTemplateColumns = "repeat(9,11.11111%)";
    this.sudokuboard.style.gridTemplateRows = "repeat(9,11.11111%)";

    for (let i = 0; i < 81; i++) {
      let div = document.createElement("div");
      div.className = "sudoku-aa-square";
      div.style.position = "relative";
      div.style.display = "flex";
      div.style.alignItems = "center";
      div.style.justifyContent = "center";
      div.style.fontFamily = "sans-serif";
      div.style.fontSize = "40px";
      div.style.border = "1px solid black";
      this.sudokuboard.appendChild(div);
    }

    this.container.appendChild(this.boxoutline)
    this.container.appendChild(this.sudokuboard)
    main_div.append(this.container);

    this.sudoku_aa.dispatcher.addEventListener("sudoku-aa-solution", this.update());
    this.sudoku_aa.dispatcher.addEventListener("sudoku-aa-searching", this.clearHandler());
  }

  update() {
    let that = this;
    return function(event) {
      let solution = event.detail.solution;
      let grid = event.detail.grid;
      for (let i=0; i<81; ++i) {
        let div = that.sudokuboard.children[i]
        div.textContent = solution[i];
        if (solution[i] == grid[i]){
          div.style.color = "black";
        }
        else {
          div.style.color = "blue";
        }
      }
    };
  }

  clear() {
    for (let i=0; i<81; ++i) {
      this.sudokuboard.children[i].textContent = "";
    }
  }

  clearHandler() {
    let that = this;
    return function() {
      that.clear();
    };
  }
}