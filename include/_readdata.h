#pragma once
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

class ReadData {
 public:
  static ReadData& getInstance() { return readData; }
  ReadData() {
    read_fpga_out();
    read_info();
    read_net();
    read_topo();
  };
  // 每个FPGA所包含的结点
  std::unordered_map<int, std::set<int>> fpga_node_map;
  // 每个结点所属于的fpga
  std::unordered_map<int, int> node_fpga_map;
  // 每个net的源点和汇点 包含delay
  std::map<int, std::vector<std::pair<int, double>>> net_src_dst_map;

  // FPGA对外最大通道数量限制
  std::unordered_map<int, int> fpga_cha_limit;
  // 整个拓扑结构
  std::vector<std::vector<int>> topo_mat;

  // 每个fpga的对外连接数
  std::unordered_map<int, int> fpga_out_conn_count;
  // net数量
  int net_num{0};

  // 每组net对应的maxdelay
  // std::unordered_map<int,double>net_max_delay_map;
  std::vector<std::pair<int, double>> net_delay_vec;

  // 每组net的fpga所包含的结点
  std::unordered_map<int, std::unordered_map<int, std::vector<int>>>
      net_fpga_node_map;
  // 每组net的结点所对应的路径
  std::unordered_map<int,
                     std::unordered_map<int, std::vector<std::array<int, 2>>>>
      net_node_path_map;
  // 每条边的所经过的net数量 (1-based索引)
  int edge_net_count[66][66]{};

  // FPGA数量
  int Fpga_num{0};
  // 通道总数
  int tot_channel_count{0};
  // 一组net最多包含的结点
  int max_net_node_count{0};
  // 读取每个fpga所包含的结点
  bool read_fpga_out();
  // 读取每个Fpga的对外最大通道数量限制
  bool read_info();
  // 读取每组net的源点和汇点
  bool read_net();
  // 读取整个拓扑结构
  bool read_topo();

  ~ReadData() {};
  static ReadData readData;
  ReadData(const ReadData&) = delete;
  ReadData& operator=(const ReadData&) = delete;
};
