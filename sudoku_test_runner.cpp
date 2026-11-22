#include "sudokuBoard.h"
#include "sudokuSolver.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct TestCase {
    std::string id;
    int size{};
    std::string category;
    bool expectedValid{};
    bool expectedSolvable{};
    std::string grid;
    std::string description;
};

static bool parseBool(const std::string& value) {
    return value == "TRUE" || value == "true" || value == "1";
}

static std::vector<std::vector<int>> parseGrid(const std::string& encoded, int expectedSize) {
    std::vector<std::vector<int>> grid;
    std::stringstream rows(encoded);
    std::string rowText;
    while (std::getline(rows, rowText, '/')) {
        std::vector<int> row;
        std::stringstream cells(rowText);
        std::string cell;
        while (std::getline(cells, cell, ',')) {
            if (cell == "." || cell.empty()) row.push_back(-1);
            else row.push_back(std::stoi(cell));
        }
        grid.push_back(std::move(row));
    }
    if (static_cast<int>(grid.size()) != expectedSize) return {};
    for (const auto& row : grid)
        if (static_cast<int>(row.size()) != expectedSize) return {};
    return grid;
}

static bool isCompleteAndPreservesClues(const std::vector<std::vector<int>>& original,
                                        const std::vector<std::vector<int>>& solved,
                                        int n) {
    if (static_cast<int>(solved.size()) != n) return false;
    const int box = static_cast<int>(std::sqrt(n));
    for (int r = 0; r < n; ++r) {
        if (static_cast<int>(solved[r].size()) != n) return false;
        std::vector<bool> rowSeen(n + 1, false), colSeen(n + 1, false);
        for (int c = 0; c < n; ++c) {
            const int a = solved[r][c];
            const int b = solved[c][r];
            if (a < 1 || a > n || rowSeen[a]) return false;
            if (b < 1 || b > n || colSeen[b]) return false;
            rowSeen[a] = true;
            colSeen[b] = true;
            if (original[r][c] != -1 && original[r][c] != a) return false;
        }
    }
    for (int br = 0; br < n; br += box) {
        for (int bc = 0; bc < n; bc += box) {
            std::vector<bool> seen(n + 1, false);
            for (int r = br; r < br + box; ++r)
                for (int c = bc; c < bc + box; ++c) {
                    int v = solved[r][c];
                    if (v < 1 || v > n || seen[v]) return false;
                    seen[v] = true;
                }
        }
    }
    return true;
}

static bool loadCases(const std::string& filename, std::vector<TestCase>& tests) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open '" << filename << "'.\n"
                  << "Place sudoku_testcases.csv beside the test runner or pass its path.\n";
        return false;
    }
    std::string line;
    std::getline(file, line); // header
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        // Dataset fields are CSV-quoted by the generator; Grid and Description contain no commas
        // outside their own quoted fields. This small parser handles quoted CSV fields.
        std::vector<std::string> fields;
        std::string field;
        bool quoted = false;
        for (std::size_t i = 0; i < line.size(); ++i) {
            char ch = line[i];
            if (ch == '"') {
                if (quoted && i + 1 < line.size() && line[i + 1] == '"') { field += '"'; ++i; }
                else quoted = !quoted;
            } else if (ch == ',' && !quoted) {
                fields.push_back(field); field.clear();
            } else field += ch;
        }
        fields.push_back(field);
        if (fields.size() != 7) {
            std::cerr << "WARNING: Skipping malformed CSV row: " << line << '\n';
            continue;
        }
        tests.push_back({fields[0], std::stoi(fields[1]), fields[2],
                         parseBool(fields[3]), parseBool(fields[4]), fields[5], fields[6]});
    }
    return true;
}

int main(int argc, char* argv[]) {
    const std::string filename = argc > 1 ? argv[1] : "sudoku_testcases.csv";
    std::vector<TestCase> tests;
    if (!loadCases(filename, tests)) return 1;

    solver sudokuSolver;
    int passed = 0, failed = 0, skipped = 0;
    std::cout << "============================================================\n"
              << "             SUDOKU SOLVER - AUTOMATED TEST RUNNER\n"
              << "============================================================\n"
              << "Dataset : " << filename << "\n"
              << "Loaded  : " << tests.size() << " test cases\n\n";

    for (const auto& test : tests) {
        try {
            auto grid = parseGrid(test.grid, test.size);
            if (grid.empty()) {
                ++skipped;
                std::cout << "[SKIP] " << test.id << " - malformed " << test.size << "x" << test.size << " grid\n";
                continue;
            }
            board problem(grid);
            const bool actualValid = sudokuSolver.isValid(problem);
            const auto solved = sudokuSolver.solveGrid(problem);
            const bool actualSolvable = !solved.empty();

            bool solutionCorrect = true;
            if (actualSolvable)
                solutionCorrect = isCompleteAndPreservesClues(grid, solved, test.size);

            const bool validCheck = actualValid == test.expectedValid;
            const bool solvableCheck = actualSolvable == test.expectedSolvable;
            const bool passedCase = validCheck && solvableCheck && (!actualSolvable || solutionCorrect);

            if (passedCase) {
                ++passed;
                std::cout << "[PASS] " << test.id << " | " << test.size << "x" << test.size
                          << " | " << test.category << '\n';
            } else {
                ++failed;
                std::cout << "[FAIL] " << test.id << " | " << test.size << "x" << test.size
                          << " | " << test.category << '\n'
                          << "       Expected valid/solvable: " << test.expectedValid << "/" << test.expectedSolvable
                          << " | Actual: " << actualValid << "/" << actualSolvable
                          << " | Solution integrity: " << (actualSolvable ? (solutionCorrect ? "OK" : "BROKEN") : "N/A")
                          << '\n';
            }
        } catch (const std::exception& e) {
            ++failed;
            std::cout << "[FAIL] " << test.id << " | Exception: " << e.what() << '\n';
        }
    }

    const int total = passed + failed + skipped;
    const double rate = total ? (100.0 * passed / total) : 0.0;
    std::cout << "\n============================================================\n"
              << "                    TEST SUMMARY\n"
              << "============================================================\n"
              << "Total test cases : " << total << '\n'
              << "Passed           : " << passed << '\n'
              << "Failed           : " << failed << '\n'
              << "Skipped          : " << skipped << '\n'
              << "Pass rate        : " << rate << "%\n"
              << "============================================================\n";
    return failed == 0 && skipped == 0 ? 0 : 1;
}
