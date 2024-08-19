#include <iostream>
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

choice<matrix<choice<cell>>> determine_first(matrix<choice<cell>>& m, size_t d) {
  for (size_t i = 0; i < d*d; i++)
    for (size_t j = 0; j < d*d; j++)
      if (m[i][j].size() > 1)
	return determine_cell(m, j, i, d);
  return {};
}

std::list<matrix<cell>> solutions(matrix<choice<cell>>& m, size_t d) {
  std::list<matrix<cell>> result;

  std::list<matrix<choice<cell>>> stack = {m};

  while (!stack.empty()) {
    matrix<choice<cell>> ch = stack.front();
    stack.pop_front();
    int status = grid_status(ch);
    if (status == 1) result.push_back(remove_choice(ch));
    else if (status == 0) {
      auto dets = determine_first(ch, d);
      for (auto &det : dets) stack.push_back(det);
    }
  }
  
  return result;
}

int main (int argc, char *argv[]) {
  std::string output_filename = "result.txt";
  std::string input_filename;
  if (argc == 1) {
    std::cout << "Usage: " << argv[0] << " <filename>" << std::endl;
    std::cout << "    -o <filename> | specifices output file." << std::endl;
    return 0;
  }

  for (size_t i = 1; i < argc; i++) {
    if (argv[i][0] != '-') {
      input_filename = argv[i];
      continue;
    }

    // TODO: Check is there is one more paramater for flag.
    switch (argv[i][1]) {
      case 'o': output_filename = argv[++i]; break;

      default:
        std::cout << "Unexpected flag '" << argv[i][0] << "'. Run '" << argv[0] << "' for help." << std::endl;
    }
    

  }

  std::ifstream input_file(input_filename);
  if (!input_file.good()) {
    std::cout << "Can't access file '" << input_filename << "'." << std::endl;
    return 0;
  }

  size_t d = 3;
  matrix<choice<cell>> grid = generate_choice_matrix(d);
  
  for (int i = 0; i < d * d; i++)
    for (int j = 0; j < d * d; j++) {
      char c; input_file >> c;
      if (c != '.') determine(grid, j, i, c - '1' + 1, d);
    }
  input_file.close();

  auto begin_time = std::chrono::high_resolution_clock::now();
  auto sols = solutions(grid, d);
  auto end_time = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> calc_time = end_time - begin_time;
  
  // save to file
  std::ofstream output_file(output_filename);
  for (auto &dd : sols) {
    for (int i = 0; i < d*d; i++) {
      for (int j = 0; j < d*d; j++)
  	    output_file << dd[i][j] << ' ';
      output_file << std::endl;
    }
    output_file << std::endl;
  }
  output_file.close();

  // logs
  std::cout << "Found " << sols.size() << " solution(s) in "
	    << calc_time.count() << " seconds." << std::endl;
  return 0;
}
