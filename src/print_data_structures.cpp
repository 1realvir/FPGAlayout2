/*
AI验证调试

*/
#include "../include/_readdata.h"
#include <iostream>
#include <iomanip>

void print_fpga_node_mapping() {
    std::cout << "\n=== FPGA 节点映射 (fpga_node_map) ===" << std::endl;
    ReadData& data = ReadData::getInstance();
    
    for (const auto& [fpga_id, nodes] : data.fpga_node_map) {
        std::cout << "FPGA" << fpga_id << " 包含 " << nodes.size() << " 个节点: ";
    bool first = true;
    for (int node_id : nodes) {
        if (!first) std::cout << ", ";
        std::cout << "g" << node_id;
        first = false;
    }
        std::cout << std::endl;
    }
    std::cout << "总计: " << data.fpga_node_map.size() << " 个 FPGA" << std::endl;
}

void print_node_fpga_mapping() {
    std::cout << "\n=== 节点 FPGA 映射 (node_fpga_map) ===" << std::endl;
    ReadData& data = ReadData::getInstance();
    
    std::cout << "节点 -> FPGA 映射:" << std::endl;
    bool first = true;
    for (const auto& [node_id, fpga_id] : data.node_fpga_map) {
        if (!first) std::cout << ", ";
        std::cout << "g" << node_id << " -> FPGA" << fpga_id;
        if ((node_id % 8) == 0) std::cout << std::endl; // 每8个换行
        first = false;
    }
    std::cout << std::endl;
    std::cout << "总计: " << data.node_fpga_map.size() << " 个节点映射" << std::endl;
}

void print_fpga_channel_limits() {
    std::cout << "\n=== FPGA 通道限制 (fpga_cha_limit) ===" << std::endl;
    ReadData& data = ReadData::getInstance();
    
    for (const auto& [fpga_id, limit] : data.fpga_cha_limit) {
        std::cout << "FPGA" << fpga_id << " 通道限制: " << limit << std::endl;
    }
    std::cout << "总计: " << data.fpga_cha_limit.size() << " 个 FPGA 限制" << std::endl;
}

void print_network_connections() {
    std::cout << "\n=== 网络连接 (net_src_dst_map) ===" << std::endl;
    ReadData& data = ReadData::getInstance();
    
    for (const auto& [net_id, nodes] : data.net_src_dst_map) {
        std::cout << "Net" << net_id << " (" << nodes.size() << " 个节点): ";
        for (size_t i = 0; i < nodes.size(); i++) {
            std::cout << "g" << nodes[i].first;
            if (i == 0) std::cout << "(源)";
            if (i < nodes.size() - 1) std::cout << " -> ";
        }
        std::cout << std::endl;
    }
    std::cout << "总计: " << data.net_src_dst_map.size() << " 个网络" << std::endl;
    std::cout << "net_num: " << data.net_num << std::endl;
}

void print_net_fpga_mapping() {
    std::cout << "\n=== 网络 FPGA 映射 (net_fpga_node_map) ===" << std::endl;
    ReadData& data = ReadData::getInstance();
    
    for (const auto& [net_id, fpga_map] : data.net_fpga_node_map) {
        std::cout << "Net" << net_id << " 涉及的 FPGA:" << std::endl;
        for (const auto& [fpga_id, nodes] : fpga_map) {
            std::cout << "  FPGA" << fpga_id << ": ";
            for (size_t i = 0; i < nodes.size(); i++) {
                std::cout << "g" << nodes[i];
                if (i < nodes.size() - 1) std::cout << ", ";
            }
            std::cout << std::endl;
        }
    }
}

void print_topology_matrix() {
    std::cout << "\n=== 拓扑矩阵 (topo_mat) ===" << std::endl;
    ReadData& data = ReadData::getInstance();
    
    std::cout << "FPGA 连接矩阵:" << std::endl;
    std::cout << "     ";
    for (size_t j = 1; j < data.topo_mat[0].size(); j++) {
        std::cout << "F" << std::setw(2) << j << " ";
    }
    std::cout << std::endl;
    
    for (size_t i = 1; i < data.topo_mat.size(); i++) {
        std::cout << "F" << std::setw(2) << i << ": ";
        for (size_t j = 1; j < data.topo_mat[i].size(); j++) {
            std::cout << std::setw(3) << data.topo_mat[i][j] << " ";
        }
        std::cout << std::endl;
    }
    
    std::cout << "FPGA 数量: " << data.Fpga_num << std::endl;
    std::cout << "总通道数量: " << data.tot_channel_count << std::endl;
}

void print_statistics() {
    std::cout << "\n=== 统计信息 ===" << std::endl;
    ReadData& data = ReadData::getInstance();
    
    std::cout << "FPGA 数量: " << data.Fpga_num << std::endl;
    std::cout << "网络数量: " << data.net_num << std::endl;
    std::cout << "总通道数量: " << data.tot_channel_count << std::endl;
    std::cout << "最大网络节点数: " << data.max_net_node_count << std::endl;
    
    // 计算每个FPGA的节点数
    std::cout << "\n各 FPGA 节点分布:" << std::endl;
    for (const auto& [fpga_id, nodes] : data.fpga_node_map) {
        std::cout << "FPGA" << fpga_id << ": " << nodes.size() << " 个节点" << std::endl;
    }
    
    // 计算网络大小分布
    std::cout << "\n网络大小分布:" << std::endl;
    std::map<int, int> size_distribution;
    for (const auto& [net_id, nodes] : data.net_src_dst_map) {
        size_distribution[nodes.size()]++;
    }
    for (const auto& [size, count] : size_distribution) {
        std::cout << size << " 个节点的网络: " << count << " 个" << std::endl;
    }
}

void verify_data_consistency() {
    std::cout << "\n=== 数据一致性验证 ===" << std::endl;
    ReadData& data = ReadData::getInstance();
    
    // 验证 fpga_node_map 和 node_fpga_map 的一致性
    bool consistent = true;
    for (const auto& [fpga_id, nodes] : data.fpga_node_map) {
        for (int node_id : nodes) {
            auto it = data.node_fpga_map.find(node_id);
            if (it == data.node_fpga_map.end() || it->second != fpga_id) {
                std::cout << "❌ 不一致: g" << node_id << " 在 fpga_node_map 中属于 FPGA" 
                         << fpga_id << "，但在 node_fpga_map 中属于 FPGA" 
                         << (it != data.node_fpga_map.end() ? it->second : -1) << std::endl;
                consistent = false;
            }
        }
    }
    
    if (consistent) {
        std::cout << "✅ fpga_node_map 和 node_fpga_map 一致" << std::endl;
    }
    
    // 验证网络中的节点都在 node_fpga_map 中
    bool all_nodes_found = true;
    for (const auto& [net_id, nodes] : data.net_src_dst_map) {
        for (const auto& [node_id, delay] : nodes) {
            if (data.node_fpga_map.find(node_id) == data.node_fpga_map.end()) {
                std::cout << "❌ Net" << net_id << " 中的节点 g" << node_id << " 未在 node_fpga_map 中找到" << std::endl;
                all_nodes_found = false;
            }
        }
    }
    
    if (all_nodes_found) {
        std::cout << "✅ 所有网络节点都在 node_fpga_map 中" << std::endl;
    }
    
    // 验证拓扑矩阵大小
    if (data.topo_mat.size() == data.Fpga_num + 1) {
        std::cout << "✅ 拓扑矩阵大小正确 (" << data.topo_mat.size() << " 行)" << std::endl;
    } else {
        std::cout << "❌ 拓扑矩阵大小错误: 期望 " << data.Fpga_num + 1 
                 << " 行，实际 " << data.topo_mat.size() << " 行" << std::endl;
    }
}

int main() {
    std::cout << "=== ReadData 数据结构验证程序 ===" << std::endl;
    std::cout << "======================================" << std::endl;
    
    // 创建 ReadData 实例，这会自动读取所有数据
    ReadData& data = ReadData::getInstance();
    
    // 打印所有数据结构
    print_fpga_node_mapping();
    print_node_fpga_mapping();
    print_fpga_channel_limits();
    print_network_connections();
    print_net_fpga_mapping();
    print_topology_matrix();
    print_statistics();
    verify_data_consistency();
    
    std::cout << "\n=== 验证完成 ===" << std::endl;
    
    return 0;
}
