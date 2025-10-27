#include "../include/_readdata.h"
ReadData ReadData::readData;
// #include <limits.h>
// #include <unistd.h>

// #include <iostream>
/*
F1: g13 g14 g15 g16
F2: g1 g2 g7 g9
F3: g4 g5 g6 g10 g12
F4: g3 g8 g11
*/
bool ReadData::read_fpga_out() {
  // //输出当前目录
  // char cwd[PATH_MAX];
  // if (getcwd(cwd, sizeof(cwd)) != nullptr) {
  //   std::cout << "当前目录: " << cwd << std::endl;
  // } else {
  //   perror("getcwd() 错误");
  // }
  // return 0;
  std::ifstream inputfile("public/case01/design.fpga.out");

  if (inputfile.is_open()) {
    //  std::cout<<1<<std::endl;
    std::string line;
    while (std::getline(inputfile, line)) {
      for (int i = 0; i < line.size(); i++) {
        if (line[i] == ':') {
          int fpga_id = std::stoi(line.substr(1, i - 1));
          std::set<int> node_set;
          std::string nodes = line.substr(i + 2);
          std::istringstream ss(nodes);
          std::string node;
          while (ss >> node) {
            int node_id = std::stoi(node.substr(1));
            node_set.insert(node_id);
            node_fpga_map[node_id] = fpga_id;
          }
          fpga_node_map[fpga_id] = node_set;
        }
      }
    }
    return true;
  } else {
    std::cerr << "Failed to open the file::design.fpga.out" << std::endl;
    return false;
  }
}
/*
F1 3
F2 3
F3 3
F4 3

*/
bool ReadData::read_info() {
  std::ifstream inputfile("public/case01/design.info");
  if (inputfile.is_open()) {
    // std::cout << 2 << std::endl;
    std::string line;
    while (std::getline(inputfile, line)) {
      std::istringstream ss(line);
      std::string fpga;
      int lim;
      ss >> fpga >> lim;
      int fpga_id = std::stoi(fpga.substr(1));
      fpga_cha_limit[fpga_id] = lim;
    }
    return true;
  } else {
    std::cerr << "Failed to open the file::design.info" << std::endl;
    return false;
  }
}
/*
g1 1 g2
g2 1 g4 g5
g3 1 g4 g5 g8
g4 1 g9 g10
g6 1 g2 g3 g4 g7
g8 1 g6 g10
g9 1 g1
g9 1 g2
g9 1 g12
g11 1 g5 g6 g9
g12 1 g5 g7 g9 g10 g14 g15
g13 1 g3 g16
g15 1 g16
*/
/*net_fpga_node_map*/
bool ReadData::read_net() {
  std::ifstream inputfile("public/case01/design.net");
  if (inputfile.is_open()) {
    //  std::cout << 3 << std::endl;
    std::string line;
    int line_num{};
    while (std::getline(inputfile, line)) {
      line_num++;
      for (int i = 1; i < line.size(); i++) {
        if (line[i] == '1' && line[i - 1] == ' ') {
          int net_id = line_num;
          int ori_id = std::stoi(line.substr(1, i - 2));

          std::vector<std::pair<int, double>> src_dst_vec;

          src_dst_vec.push_back(std::make_pair(ori_id, 0.0));

          std::string nodes = line.substr(i + 2);
          std::istringstream ss(nodes);
          std::string node;
          while (ss >> node) {
            int node_id = std::stoi(node.substr(1));
            net_fpga_node_map[net_id][node_fpga_map[node_id]].push_back(
                node_id);
            src_dst_vec.push_back(std::make_pair(node_id, 0));
          }
          net_src_dst_map[net_id] = src_dst_vec;
          max_net_node_count =
              std::max(max_net_node_count,
                       static_cast<int>(src_dst_vec.size()));
        }
      }
    }
    net_num = line_num;
    return true;

  } else {
    std::cerr << "Failed to open the file::design.net" << std::endl;
    return false;
  }
}
/*
F1: 0,1,0,1
F2: 1,0,1,0
F3: 0,1,0,1
F4: 1,0,1,0
*/
bool ReadData::read_topo() {
  std::ifstream inputfile("public/case01/design.topo");
  if (inputfile.is_open()) {
    // std::cout << 4 << std::endl;
    std::string line;
    int num{};
    topo_mat.push_back(std::vector<int>());  // 占位，使索引从1开始
    while (std::getline(inputfile, line)) {
      std::vector<int> row;
      num++;
      row.push_back(0);  // 保持索引从1开始
      for (int i = 0; i < line.size(); i++) {
        if (line[i] == ':') {
          std::string values = line.substr(i + 2);
          std::istringstream ss(values);
          std::string value;
          while (std::getline(ss, value, ',')) {
            //记录通道总数
            tot_channel_count += std::stoi(value);
            row.push_back(std::stoi(value));
          }
          topo_mat.push_back(row);
        }
      }
    }
    Fpga_num = num;
    return true;
  } else {
    std::cerr << "Failed to open the file::design.topo" << std::endl;
    return false;
  }
}
