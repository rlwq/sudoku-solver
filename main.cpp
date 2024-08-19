#include <iostream>
#include <iomanip>
#include <vector>
#include <list>
#include <fstream>
#include <chrono>

template<typename T> using choice = std::list<T>;
template<typename T> using matrix = std::vector<std::vector<T>>;

using cell = size_t;

choice<cell> generate_choice(size_t d) {
  choice<cell> result;
  for (cell i = 1; i <= d*d; i++) result.push_back(i);
  return result;
}

matrix<choice<cell>> generate_choice_matrix(size_t d) {
  matrix<choice<cell>> result(d*d, std::vector<choice<cell>>(d*d));
  for (size_t i = 0; i < d*d; i++)
    for (size_t j = 0; j < d*d; j++)
      for (cell n = 1; n <= d*d; n++)
	result[i][j].push_back(n);
  return result;
}

void remove_collisions(matrix<choice<cell>>& m, size_t x, size_t y, size_t d) {
  if (m[y][x].size() != 1) {
    std::cout << "Incorrect remove_collisions call!" << std::endl;
    return;
  }
  cell v = m[y][x].front();
  for (size_t i = 0; i < m.size(); i++) {
    // going through X axis
    if (i != x && (m[y][i].size() != 1 || m[y][i].front() == v)) {
      m[y][i].remove(v);
      if (m[y][i].size() == 1) remove_collisions(m, i, y, d);
    }
    
    // going through Y axis
    if (i != y && (m[i][x].size() != 1 || m[i][x].front() == v)) {
      m[i][x].remove(v);
      if (m[i][x].size() == 1) remove_collisions(m, x, i, d);
    }
    
    // going through the sub square
    size_t sq_x = d * (x / d) + i % d, sq_y = d * (y / d) + i / d;
    if ((sq_x != x || sq_y != y) && (m[sq_y][sq_x].size() != 1 || m[sq_y][sq_x].front() == v)) {
      m[sq_y][sq_x].remove(v);
      if (m[sq_y][sq_x].size() == 1) remove_collisions(m, sq_x, sq_y, d);
    }
  }
}

inline void determine(matrix<choice<cell>>& m, size_t x, size_t y, cell value, size_t d) {
  m[y][x] = {value};
  remove_collisions(m, x, y, d);
}

choice<matrix<choice<cell>>> determine_cell(matrix<choice<cell>>& m, size_t x, size_t y, size_t d) {
  if (m[y][x].size() == 1) return {m};
  
  choice<matrix<choice<cell>>> result;

  for (cell &v : m[y][x]) {
    matrix<choice<cell>> det = m;
    determine(det, x, y, v, d);
    result.push_back(det);
  }

  return result;
}

matrix<cell> remove_choice(matrix<choice<cell>>& m) {
  matrix<cell> result(m.size(), std::vector<cell>(m[0].size()));
  for (size_t i = 0; i < m.size(); i++)
    for (size_t j = 0; j < m.size(); j++)
      result[i][j] = m[i][j].front();
  return result;
}

int grid_status(matrix<choice<cell>>& m) {
  bool solved = true;

  for (auto &r : m)
    for (auto &v : r)
      if (v.size() == 0) return -1;
      else if (v.size() != 1) solved = false;
  
  return solved ? 1 : 0;
}

choice<matrix<choice<cell>>> determine_one(matrix<choice<cell>>& m, size_t d) {
  size_t det_x, det_y, curr_size = SIZE_MAX;
  bool found = false;

  for (size_t i = 0; i < d*d; i++)
    for (size_t j = 0; j < d*d; j++)
      if (m[i][j].size() > 1 && m[i][j].size() < curr_size) {
        det_x = j; det_y = i;
        curr_size = m[i][j].size();
        found = true;
      }
  if (!found) return {};
	return determine_cell(m, det_x, det_y, d);
}

std::list<matrix<cell>> get_solutions(matrix<choice<cell>>& m, size_t d, size_t solution_count_limit) {
  std::list<matrix<cell>> result;

  std::list<matrix<choice<cell>>> stack = {m};

  while (!stack.empty() && (solution_count_limit == 0 || result.size() < solution_count_limit)) {
    matrix<choice<cell>> ch = stack.front();
    stack.pop_front();
    int status = grid_status(ch);
    if (status == 1) result.push_back(remove_choice(ch));
    else if (status == 0) {
      auto dets = determine_one(ch, d);
      for (auto &det : dets) stack.push_back(det);
    }
  }
  
  return result;
}

cell parse_cell (char c) {
  if ('1' <= c && c <= '9') return c - '0';
  if ('a' <= c && c <= 'z') return 10 + c - 'a';
  if ('A' <= c && c <= 'Z') return 36 + c - 'A';
  return 0;
}

char write_cell (cell c) {
  if ( 1 <= c && c <=  9) return c + '0';
  if (10 <= c && c <= 35) return c - 10 + 'a';
  if (36 <= c && c <= 61) return c - 36 + 'A';
  return '\0';
}

int main (int argc, char *argv[]) {
  std::string output_filename = "result.txt";
  std::string input_filename;
  size_t grid_dimension = 3;
  size_t solution_count_limit = 0;

  if (argc == 1) {
    std::cout << "Usage: " << argv[0] << " <filename>" << std::endl;
    std::cout << "    -o <filename> | specifices the output file." << std::endl;
    std::cout << "    -c <integer>  | determines the limit on the number of solutions." << std::endl;
    std::cout << "    -d <integer>  | determines the dimension of the sudoku." << std::endl;
    return 0;
  }

  for (size_t i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      input_filename = argv[i];
      continue;
    }

    switch (argv[i][1]) {
      case 'o': output_filename = argv[++i]; break;
      case 'd': grid_dimension = atoi(argv[++i]); break;
      case 'c': solution_count_limit = atoi(argv[++i]); break;

      default:
        std::cout << "Unexpected flag '" << argv[i][0] << "'. Run '" << argv[0] << "' for help." << std::endl;
    }
    

  }

  size_t grid_dimension_sq = grid_dimension * grid_dimension;
  std::ifstream input_file(input_filename);
  if (!input_file.good()) {
    std::cout << "Can't access file '" << input_filename << "'." << std::endl;
    return 0;
  }

  matrix<choice<cell>> grid = generate_choice_matrix(grid_dimension);
  
  // read file
  for (int i = 0; i < grid_dimension_sq; i++)
    for (int j = 0; j < grid_dimension_sq; j++) {
      char c;
      if (!(input_file >> c)) {
        std::cout << "Not enough characters in file!" << std::endl;
        input_file.close();
        return 0;
      }
      if (c == '.') continue;

      cell parsed = parse_cell(c);
      if (parsed == 0 || parsed > grid_dimension_sq) {
        std::cout << "Unexpected character '" << c << "'." << std::endl;
        input_file.close();
        return 0;
      }
      determine(grid, j, i, parsed, grid_dimension);
    }
  input_file.close();

  // finding solutions...
  auto begin_time = std::chrono::high_resolution_clock::now();
  auto solutions = get_solutions(grid, grid_dimension, solution_count_limit);
  auto end_time = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> calc_time = end_time - begin_time;
  
  // save to file

  // TODO: remove redundant spaces and add a flag for removing them at all.
  std::ofstream output_file(output_filename);
  for (auto &dd : solutions) {
    for (int i = 0; i < grid_dimension_sq; i++) {
      for (int j = 0; j < grid_dimension_sq; j++)
  	    output_file << write_cell(dd[i][j]) << ' ';
      output_file << std::endl;
    }
    output_file << std::endl;
  }
  output_file.close();

  // logs
  std::cout << std::fixed << std::setprecision(4)
            << "Found " << solutions.size() << " solution(s) in "
	          << calc_time.count() << " seconds." << std::endl;
  return 0;
}
