//Program: EECS 348 Assignment 6
//Description: C++ program that solves Sudoku puzzles, providing all valid solutions, or displaying no solution if there are no solutions
//Input: Puzzle text files puzzle1.txt through puzzle5.txt
//Output: Terminal output of the solved puzzles and one combined solutions.txt file
//Author: taken from GTA Tao Zhe due to not recieving it from intended sender
//Collaborators: Claude
//Creation Date: unknown
//Revision Date: 11/13/26
//Revised by: Isaac King
//Revisions: Added file output functionality to write solutions to solutionN.txt files


#include <iostream> // Include standard input-output stream library for console printing and debugging
#include <fstream>  // Include file stream library for reading input puzzle files and writing solution files
#include <vector>   // Include vector container library for storing filenames and collected puzzle solutions
#include <string>   // Include string library for handling filenames, paths, and text formatting
#include <array>    // Include array library for fixed-size 2D solution storage


class SudokuSolver { // Begin definition of the SudokuSolver class
private: // Declare private access specifier for internal data members and helper methods
    int grid[9][9]; // 2D integer array (9x9) storing digits 1-9 for filled cells and 0 for empty cells
    std::vector<std::array<std::array<int, 9>, 9>> solutions; // Vector storing all valid 9x9 solution grids found during backtracking


    bool isSafe(int row, int col, int num) const { // Define constant helper method accepting cell coordinates and candidate digit
        for (int i = 0; i < 9; ++i) { // Iterate index i from 0 to 8 to inspect the row, column, and 3x3 block simultaneously
            if (grid[row][i] == num) { // Check if the candidate digit already exists in the current row
                return false; // Return false immediately because the row uniqueness constraint is violated
            } // End of row constraint check
            if (grid[i][col] == num) { // Check if the candidate digit already exists in the current column
                return false; // Return false immediately because the column uniqueness constraint is violated
            } // End of column constraint check
            int boxRow = 3 * (row / 3) + (i / 3); // Compute the row index of the i-th cell inside the current 3x3 sub-grid
            int boxCol = 3 * (col / 3) + (i % 3); // Compute the column index of the i-th cell inside the current 3x3 sub-grid
            if (grid[boxRow][boxCol] == num) { // Check if the candidate digit already exists in the 3x3 sub-grid block
                return false; // Return false immediately because the 3x3 block uniqueness constraint is violated
            } // End of 3x3 block constraint check
        } // End of loop over all 9 positions in the row, column, and 3x3 block
        return true; // Return true when no conflicts exist in the row, column, or 3x3 block
    } // End of isSafe method


    bool isInitialBoardValid() { // Define method to validate all pre-existing clues on the board
        for (int r = 0; r < 9; ++r) { // Loop through each row r from 0 to 8
            for (int c = 0; c < 9; ++c) { // Loop through each column c from 0 to 8
                if (grid[r][c] != 0) { // Check if the current cell contains a pre-filled digit (non-zero)
                    int temp = grid[r][c]; // Store the pre-filled digit temporarily in variable temp
                    grid[r][c] = 0; // Temporarily clear the cell to 0 so isSafe does not compare the cell against itself
                    bool valid = isSafe(r, c, temp); // Check if the pre-filled digit conflicts with any other cell
                    grid[r][c] = temp; // Restore the original pre-filled digit back into the cell at (r, c)
                    if (!valid) { // Check if a conflict was detected among the initial clues
                        return false; // Return false immediately if the starting puzzle is invalid
                    } // End of conflict check conditional
                } // End of non-zero cell check
            } // End of column loop
        } // End of row loop
        return true; // Return true if all pre-filled clues satisfy Sudoku constraints
    } // End of isInitialBoardValid method


    void backtrack(int row, int col) { // Define recursive method taking the current cell coordinates (row, col)
        if (row == 9) { // Base case: check if recursion has progressed past the final row (row index 8)
            std::array<std::array<int, 9>, 9> currentSolution; // Declare a 2D array to hold the completed solution grid
            for (int r = 0; r < 9; ++r) { // Loop through each row r from 0 to 8
                for (int c = 0; c < 9; ++c) { // Loop through each column c from 0 to 8
                    currentSolution[r][c] = grid[r][c]; // Copy each grid cell into the current solution array
                } // End of column copy loop
            } // End of row copy loop
            solutions.push_back(currentSolution); // Append the discovered solution grid into the solutions vector
            return; // Return to the caller to continue backtracking and search for any additional solutions
        } // End of base case conditional

        int nextRow = (col == 8) ? (row + 1) : row; // Calculate the row coordinate for the next cell in row-major order
        int nextCol = (col == 8) ? 0 : (col + 1); // Calculate the column coordinate for the next cell in row-major order

        if (grid[row][col] != 0) { // Check if the current cell at (row, col) is already filled with a clue
            backtrack(nextRow, nextCol); // Recursively advance to the next cell without altering the pre-filled cell
            return; // Return once all recursive branches from the next cell have completed
        } // End of pre-filled cell check

        for (int num = 1; num <= 9; ++num) { // Iterate candidate digit num from 1 through 9
            if (isSafe(row, col, num)) { // Check if placing candidate digit num at (row, col) satisfies all constraints
                grid[row][col] = num; // Place the valid candidate digit num into the cell at (row, col)
                backtrack(nextRow, nextCol); // Recursively invoke backtrack on the next cell with k+1 cells filled
                grid[row][col] = 0; // Backtrack: reset the cell at (row, col) back to 0 to try the next candidate
            } // End of valid candidate placement conditional
        } // End of candidate digit loop
    } // End of backtrack method

public: // Declare public access specifier for constructor and external interface methods

    SudokuSolver() { // Define default constructor for SudokuSolver
        solutions.clear(); // Clear any existing items in the solutions vector
        for (int r = 0; r < 9; ++r) { // Loop over each row index r from 0 to 8
            for (int c = 0; c < 9; ++c) { // Loop over each column index c from 0 to 8
                grid[r][c] = 0; // Initialize each grid cell at (r, c) to 0 (empty)
            } // End of column initialization loop
        } // End of row initialization loop
    } // End of SudokuSolver constructor


    bool loadPuzzle(const std::string& filename) { // Define method to load a puzzle file by constant string reference
        std::ifstream in(filename); // Open an input file stream for the specified puzzle filename
        if (!in.is_open()) { // Check if the input file stream failed to open
            return false; // Return false to indicate the file could not be opened
        } // End of file open check

        solutions.clear(); // Clear previous solutions stored in the vector for the new puzzle file
        char c; // Declare character variable c to hold each token extracted from the file
        for (int i = 0; i < 9; ++i) { // Loop over all 9 rows of the puzzle
            for (int j = 0; j < 9; ++j) { // Loop over all 9 columns of the puzzle
                if (!(in >> c)) { // Extract the next whitespace-delimited character and check for read failure
                    in.close(); // Close the file stream before returning on error
                    return false; // Return false to indicate incomplete or corrupted puzzle file data
                } // End of stream extraction check
                if (c == '_') { // Check if the extracted character is an underscore representing a blank square
                    grid[i][j] = 0; // Assign 0 to grid[i][j] to represent an unfilled cell internally
                } else if (c >= '1' && c <= '9') { // Check if the character is a valid ASCII digit from '1' to '9'
                    grid[i][j] = c - '0'; // Convert the ASCII digit character to its integer value (1-9)
                } else { // Handle any unexpected character token in the input file
                    in.close(); // Close the file stream upon encountering an invalid character
                    return false; // Return false to reject malformed input tokens
                } // End of token classification conditional
            } // End of column reading loop
        } // End of row reading loop
        in.close(); // Close the input file stream after all 81 tokens have been read
        return true; // Return true indicating the puzzle grid was loaded successfully
    } // End of loadPuzzle method


    void printGridToStream(std::ostream& out, const std::array<std::array<int, 9>, 9>& board) const { // Define method to output a specific board to a stream
        for (int i = 0; i < 9; ++i) { // Iterate through each row index i from 0 to 8
            for (int j = 0; j < 9; ++j) { // Iterate through each column index j from 0 to 8
                if (board[i][j] == 0) { // Check if the current cell is empty (0)
                    out << "_"; // Output underscore '_' for an empty cell
                } else { // Otherwise, the cell contains a valid digit between 1 and 9
                    out << board[i][j]; // Output the integer digit stored in the board
                } // End of cell output conditional
                if (j < 8) { // Check if the current column is not the last column in the row
                    out << " "; // Output a single space separating adjacent columns
                } // End of column separator conditional
            } // End of column printing loop
            out << "\n"; // Output a newline character at the end of each row
        } // End of row printing loop
    } // End of printGridToStream method


    void solveFile(const std::string& inputFilename, std::ofstream& outFile) { // Define method to process a puzzle file and write output to combined solutions file
        std::cout << "========================================\n"; // Print top visual separator banner to console
        std::cout << "Puzzle File: " << inputFilename << "\n"; // Print the name of the input puzzle file to console
        std::cout << "========================================\n"; // Print bottom visual separator banner to console

        if (!loadPuzzle(inputFilename)) { // Attempt to load the puzzle from the file and check if it failed
            std::cout << "Error: Could not open or read " << inputFilename << "\n\n"; // Print error message to console if loading failed
            outFile << "Puzzle: " << inputFilename << "\n"; // Write puzzle filename header to solutions file
            outFile << "No solution found.\n\n"; // Write "No solution found." to the solutions file
            return; // Return early since the puzzle could not be loaded
        } // End of file load check

        std::cout << "Initial Puzzle:\n"; // Print label preceding the initial unsolved puzzle grid
        std::array<std::array<int, 9>, 9> initialGrid; // Declare 2D array for initial puzzle
        for (int r = 0; r < 9; ++r) { // Loop over rows
            for (int c = 0; c < 9; ++c) { // Loop over columns
                initialGrid[r][c] = grid[r][c]; // Copy internal grid to initialGrid
            } // End inner loop
        } // End outer loop
        printGridToStream(std::cout, initialGrid); // Display the initial puzzle grid to console

        if (isInitialBoardValid()) { // Verify that the starting clues do not violate Sudoku rules
            backtrack(0, 0); // Invoke the recursive depth-first backtracking search starting at cell (0, 0)
        } // End of initial board validity check

        outFile << "Puzzle: " << inputFilename << "\n"; // Write puzzle filename header to solutions file
        
        if (solutions.empty()) { // Check if zero valid solutions were found after completing the search
            std::cout << "\nNo solution found.\n\n"; // Print "No solution found." to console
            outFile << "No solution found.\n"; // Write "No solution found." to the solutions file
        } else { // Handle case where one or more solutions were found
            size_t numSolutions = solutions.size(); // Retrieve total count of solutions found
            for (size_t s = 0; s < numSolutions; ++s) { // Loop through each solution in the vector
                std::cout << "\nSolution " << (s + 1) << ":\n"; // Print solution header to console
                printGridToStream(std::cout, solutions[s]); // Print the solution grid to console
                printGridToStream(outFile, solutions[s]); // Write solution grid to solutions file
            } // End of solutions loop
            std::cout << "Solutions found: " << numSolutions << "\n\n"; // Print solution count summary to console
            outFile << "Solutions found: " << numSolutions << "\n\n"; // Write solution count summary to solutions file
        } // End of solutions check conditional
    } // End of solveFile method
}; // End of SudokuSolver class definition


int main() { // Define main entry point of the C++ program
    std::vector<std::string> inputFiles = { // Initialize vector holding the five required input puzzle filenames
        "puzzle1.txt", // First Sudoku puzzle input file
        "puzzle2.txt", // Second Sudoku puzzle input file
        "puzzle3.txt", // Third Sudoku puzzle input file
        "puzzle4.txt", // Fourth Sudoku puzzle input file (unsolvable test case)
        "puzzle5.txt"  // Fifth Sudoku puzzle input file
    }; // End of input filenames vector initialization

    std::ofstream outFile("solutions.txt"); // Open single output file stream for all combined puzzle solutions
    if (!outFile.is_open()) { // Check if the output file failed to open
        std::cerr << "Error: Could not create solutions.txt\n"; // Print error message to standard error stream
        return 1; // Return exit status 1 to indicate failure
    } // End of output file opening check

    SudokuSolver solver; // Instantiate a single SudokuSolver object to process all files

    for (size_t i = 0; i < inputFiles.size(); ++i) { // Loop index i through all five input puzzle files
        solver.solveFile(inputFiles[i], outFile); // Call solveFile on the solver object for the i-th puzzle, writing to combined solutions file
    } // End of file processing loop

    outFile.close(); // Close the output file stream after processing all puzzles
    return 0; // Return exit status 0 to the operating system indicating successful completion
} // End of main function