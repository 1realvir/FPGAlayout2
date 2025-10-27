#include "../include/graphinit.h"

#include <assert.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <queue>
#include <set>
#include <utility>
#include <vector>

#include "_readdata.h"

#define RG ReadData::getInstance()
//----------

// 连边信息
int h[V], e[E], ne[E], w[E], fro[E];

int F[V];

int cnt1;

int vis2[V];

// 最短路的访问数组
int st[V];

int dp[V][1 << 16];

// 输出路径的辅助数组
std::array<int, 3> pre[V][1 << 16];

// 将生成树进行重新构图
std::vector<int> G[V];

int d[V];

// dij转移使用的堆
std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>,
                    std::greater<std::pair<int, int>>>
    pq;

//---------
void init() {}

//--------------------------

// 每一次处理net时需要初始化
void Init_Graph() {
  cnt1 = 0;

  // 清空优先队列
  while (!pq.empty()) {
    pq.pop();
  }

  for (int i = 1; i <= RG.Fpga_num; i++) {
    h[i] = -1;
  }
}

// 进行图的链接
void Add_Edge(int u, int v, int c) {
  fro[cnt1] = u;
  e[cnt1] = v;
  ne[cnt1] = h[u];
  w[cnt1] = c;
  h[u] = cnt1++;
}
// 最短路进行dp转移
void dijkstra(int s) {
  for (int i = 1; i <= RG.Fpga_num; i++) {
    st[i] = 0;
  }

  while (!pq.empty()) {
    int u = pq.top().second;

    pq.pop();

    if (st[u]) continue;

    st[u] = 1;
    for (int i = h[u]; ~i; i = ne[i]) {
      int v = e[i];

      if (dp[v][s] > dp[u][s] + w[i]) {
        dp[v][s] = dp[u][s] + w[i];

        pre[v][s] = {1, u, i};

        pq.push({dp[v][s], v});
      }
    }
  }
}

void dijkstra1(int s) {
  std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>,
                      std::greater<std::pair<int, int>>>
      pq1;
  for (int i = 1; i <= RG.Fpga_num; i++) {
    d[i] = 1 << 30;
    st[i] = 0;
  }
  d[s] = 0;

  pq1.push({d[s], s});
  while (!pq1.empty()) {
    int u = pq1.top().second;
    pq1.pop();
    if (st[u]) continue;
    st[u] = 1;
    for (int i = h[u]; ~i; i = ne[i]) {
      int v = e[i];
      if (d[v] > d[u] + w[i]) {
        d[v] = d[u] + w[i];
        pq1.push({d[v], v});
      }
    }
  }
}

int Find(int x) { return x == F[x] ? x : F[x] = Find(F[x]); }
void printedge(int u, int A) {
  if (pre[u][A][0] == 1) {
    // 输出使用的边结点
    // debug
    //  std::cout<<fro[pre[u][A][2]]<<" "<<e[pre[u][A][2]]<<std::endl;
    G[fro[pre[u][A][2]]].push_back(e[pre[u][A][2]]);
    G[e[pre[u][A][2]]].push_back(fro[pre[u][A][2]]);
    printedge(pre[u][A][1], A);
  }
  if (pre[u][A][0] == 2) {
    printedge(u, pre[u][A][1]);
    printedge(u, pre[u][A][2]);
  }
}

//---------------------------

// 计算边的权值
int calc_val(int u, int v) {
  // std::cout<<"calc_val: u="<<u<<" v="<<v<<std::endl;
  // std::cout<< RG.topo_mat[u][v]<<std::endl;
  // std::cout<<RG.edge_net_count[u][v]<<std::endl;
  return std::max(0, 50 - RG.topo_mat[u][v]) +
         std::min(200, 1 + 3 * RG.edge_net_count[u][v]);
}
// TODO
void Init_Tree(int net_id) {
  for (int i = 1; i <= RG.Fpga_num; i++) {
    vis2[i] = 0;
  }
}

void printnet_path(int net_id) {
  Init_Tree(net_id);

  // 源节点所属的FPGA
  int tmp = RG.node_fpga_map[RG.net_src_dst_map[net_id][0].first];

  // arr[2]  1:fpga_id  2:path_length
  std::queue<std::pair<int, std::vector<std::array<int, 2>>>> bfs_q;
  vis2[tmp] = 1;
  std::vector<std::array<int, 2>> v;
  v.push_back({tmp, 0});
  bfs_q.push({tmp, v});
  while (!bfs_q.empty()) {
    int u = bfs_q.front().first;

    std::vector<std::array<int, 2>> ve = bfs_q.front().second;

    bfs_q.pop();
    for (auto v : G[u]) {
      if (!vis2[v]) {
        vis2[v] = 1;
        // FPGA的一条边的俩结点的对外连接数++
        // RG.fpga_out_conn_count[u]++;
        // RG.fpga_out_conn_count[v]++;

        // 该条边使用的net数量++
        RG.edge_net_count[u][v]++;

        std::vector<std::array<int, 2>> new_ve = ve;
        new_ve.push_back({v, ve.back()[1] + 1});
        // 该FPGA包含的该组net的结点的路径进行记录
        for (auto& it : RG.net_fpga_node_map[net_id][v]) {
          RG.net_node_path_map[net_id][it] = new_ve;
          //  // debug
          //    std::cout<<it<<"  ";
          //    for(auto arr:new_ve){

          //      std::cout<<arr[0]<<" "<<arr[1]<<" ";
          //   }
          //   std::cout<<std::endl;
        }

        bfs_q.push({v, new_ve});
      }
    }
  }
  // DEbug
  // todo
}

// delay= 0.7* （net数量/通道数量）+30*fpga数量
void calc_delay() {
  for (int net_id = 1; net_id <= RG.net_num; net_id++) {
    // 每组net的最大delay
    double max_net_delay{0};

    for (auto& nd : RG.net_src_dst_map[net_id]) {
      // 结点
      int it = nd.first;
      // int fpga_id=RG.node_fpga_map[it];

      // 只包含源节点的delay为0 跳过
      if (RG.net_node_path_map[net_id][it].size() == 0) continue;

      double net_delay{};

      int path_len = RG.net_node_path_map[net_id][it].size();
      for (int i = 1; i <= path_len - 1; i++) {
        int u = RG.net_node_path_map[net_id][it][i - 1][0];
        int v = RG.net_node_path_map[net_id][it][i][0];

        // 必须是8的倍数
        int val = std::ceil(RG.edge_net_count[u][v] * 1.0 / RG.topo_mat[u][v]);
        int ceil8 = ((val + 7) / 8) * 8;
        net_delay += 0.7 * ceil8;
      }

      net_delay += 30 * (path_len - 1);

      nd.second = net_delay;
      max_net_delay = std::max(max_net_delay, net_delay);
      // double delay=0.7*(net_id/RG.tot_channel_count)+30*fpga_count;
      // std::cout<<"Net "<<net_id<<" Delay: "<<delay<<std::endl;
    }
    // RG.net_max_delay_map[net_id] = max_net_delay;
    RG.net_delay_vec.push_back({net_id, max_net_delay});
  }
}

// 输出 design.route.out
// [Net ID]
// [FPGA sequence to Sink 1] [path delay]
// [FPGA sequence to Sink 2] [path delay]
// [net 1]
// [3,4] [35.6]
// [3,1] [35.6]
void print_design_route_out() {
  // 输出到 design.route.out
  std::ofstream fout("design.route.out");
  for (auto it : RG.net_delay_vec) {
    int net_id = it.first;
    double delay = it.second;
    if (delay == 0) continue;
    fout << "[net " << net_id << "]" << std::endl;
    for (auto nd : RG.net_src_dst_map[net_id]) {
      if (nd.second == 0) continue;
      int node_id = nd.first;
      double node_delay = nd.second;
      fout << "[";
      for (auto arr : RG.net_node_path_map[net_id][node_id]) {
        fout << arr[0];
        if (arr != RG.net_node_path_map[net_id][node_id].back()) {
          fout << ",";
        }
      }
      fout << "] ";
      fout << "[" << std::fixed << std::setprecision(1) << node_delay << "]"
           << std::endl;
    }
  }
  fout.close();
}

// 对net的delay进行从大到小排序
void net_delay_sort() {
  std::sort(
      RG.net_delay_vec.begin(), RG.net_delay_vec.end(),
      [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
        return a.second > b.second;
      });

  for (auto it : RG.net_delay_vec) {
    int net_id = it.first;
    double delay = it.second;
    // DEBUG
    // std::cout << "Net " << net_id << " Delay: " << delay << std::endl;
    // 每一组net 内部的结点的排序
    std::sort(
        RG.net_src_dst_map[net_id].begin(), RG.net_src_dst_map[net_id].end(),
        [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
          return a.second > b.second;
        });
  }
}

//后续优化 每个net 可能需要建立一个一颗线段树
struct SGT {
  // 线段树的node结点
  struct node {
    int l, r;
    double sum{0};
    double maxv{0};
#define ls i * 2
#define rs i * 2 + 1
    node(int l_ = 0, int r_ = 0, double sum_ = 0, double maxv_ = 0) {
      l = l_;
      r = r_;
      sum = sum_;
      maxv = maxv_;
    }
  } tr[22 * 4];
  // 需要改
  std::unordered_map<int, double> a;
  // push操作维护
  void pushup(int i) { tr[i].maxv = std::max(tr[ls].maxv, tr[rs].maxv); }
  void build(int i, int l, int r) {
    tr[i] = {l, r};
    if (l == r) {
      tr[i].sum = tr[i].maxv = a[l];
      return;
    }
    int mid = l + r >> 1;
    build(ls, l, mid);
    build(rs, mid + 1, r);
    pushup(i);
  }
  // 区间修改
  void change(int i, int l, int r, double val) {
    if (tr[i].l == tr[i].r) {
      tr[i].sum += val;
      tr[i].maxv += val;
      return;
    }
    int mid = (tr[i].l + tr[i].r) >> 1;
    if (l <= mid) change(ls, l, r, val);
    if (r > mid) change(rs, l, r, val);
    pushup(i);
  }
  // 单点查询
  double query_sum(int i, int x) {
    if (tr[i].l == tr[i].r) return tr[i].sum;
    int mid = (tr[i].l + tr[i].r) >> 1;
    if (x <= mid)
      return query_sum(ls, x);
    else
      return query_sum(rs, x);
  }
  // 区间max
  double query_max(int i, int l, int r) {
    if (l <= tr[i].l && tr[i].r <= r) return tr[i].maxv;
    double mx = 0.0;
    int mid = (tr[i].l + tr[i].r) >> 1;
    if (l <= mid) mx = std::max(mx, query_max(ls, l, r));
    if (r > mid) mx = std::max(mx, query_max(rs, l, r));
    return mx;
  }
};

// 输出 design.newtopo
void  print_design_newtopo(){
 //TODO

}

//调整优化delay

void adjust_delay(){
  //TODO
}


// 通道总数的百分之十进行分配
void fenpei() {

      if (RG.Fpga_num <= 8 || RG.max_net_node_count < 16){
        int tot = std::floor(RG.tot_channel_count * 0.3);
        //按fpga上所包含的结点从大到小分配
        
      }
      else {
        int tot=std::floor(RG.tot_channel_count*0.1);

      }
}

void solve() {
  init();

  // 关键点
  std::vector<int> key;
  for (auto net : RG.net_src_dst_map) {
    Init_Graph();
    int net_id = net.first;
    int vis[65]{};
    // std::cout<<"Processing Net ID: " << net_id << std::endl;
    for (auto nd : net.second) {
      if (vis[RG.node_fpga_map[nd.first]]) continue;
      // std::cout<<"nd.first: "<<nd.first<<std::endl;
      // std::cout<<RG.node_fpga_map[nd.first]<<std::endl;
      vis[RG.node_fpga_map[nd.first]] = 1;
      key.push_back(RG.node_fpga_map[nd.first]);
    }

    // debug
    //  for (auto it : key) {
    //    std::cout << it << " ";
    //  }
    //  std::cout << std::endl;

    // for (int i = 1; i <= RG.Fpga_num; i++) {
    //   if (vis[i]) key.push_back(i);
    // }

    // debug
    // std::cout<<"Fpga_num: "<<RG.Fpga_num<<std::endl;
    // std::cout<<"max_net_node_count: "<<RG.max_net_node_count<<std::endl;
    // small case
    if (RG.Fpga_num <= 8 || RG.max_net_node_count < 16) {
      // 使用最小斯坦纳树
      for (int i = 1; i <= RG.Fpga_num; i++) {
        for (int j = i + 1; j <= RG.Fpga_num; j++) {
          // std::cout<<"i="<<i<<" j="<<j<<std::endl;
          // std::cout<<"topo:"<<RG.topo_mat[i][j]<<std::endl;
          if (RG.topo_mat[i][j] > 0) {
            Add_Edge(i, j, calc_val(i, j));
            Add_Edge(j, i, calc_val(i, j));
            //  std::cout << "i=" << i << " j=" << j <<calc_val(i,j)<<
            //  std::endl;
          }
        }
      }
      int mx = (1 << (key.size()));

      for (int i = 1; i <= RG.Fpga_num; i++) {
        for (int j = 0; j < mx; j++) {
          pre[i][j] = {0};
          dp[i][j] = 1 << 30;
        }
      }

      // 初始化关键点
      for (int i = 0; i < key.size(); i++) {
        dp[key[i]][1 << i] = 0;
      }

      for (int sta = 1; sta < mx; sta++) {
        for (int i = 1; i <= RG.Fpga_num; i++) {
          for (int j = sta & (sta - 1); j; j = sta & (j - 1))

            if (dp[i][j] + dp[i][j ^ sta] < dp[i][sta]) {
              dp[i][sta] = dp[i][j] + dp[i][j ^ sta];

              pre[i][sta] = {2, j, j ^ sta};
            }

          if (dp[i][sta] != (1 << 30)) pq.push({dp[i][sta], i});
        }

        dijkstra(sta);
      }

      // 记录所使用的斯坦纳树的边
      printedge(key[0], mx - 1);
      printnet_path(net_id);

    } else {
      // 使用最短路

      for (int i = 1; i <= RG.Fpga_num; i++) {
        for (int j = i + 1; j <= RG.Fpga_num; j++) {
          if (RG.topo_mat[i][j] > 0) {
            Add_Edge(i, j, calc_val(i, j));
            Add_Edge(j, i, calc_val(i, j));
          }
        }
      }

      // 取出源点
      auto& src_nd = net.second.front();
      int src_it = src_nd.first;
      int src_fpga_id = RG.node_fpga_map[src_it];
      dijkstra1(src_fpga_id);

      // 为每个节点记录路径
      for (auto& nd : net.second) {
        int it = nd.first;
        int dst_fpga_id = RG.node_fpga_map[it];

        //源节点本身，路径长度为0
        if (dst_fpga_id == src_fpga_id) {
          std::vector<std::array<int, 2>> path_vec;
          path_vec.push_back({src_fpga_id, 0});
          RG.net_node_path_map[net_id][it] = path_vec;
          continue;
        }

        // 从目标节点回溯到源节点
        std::vector<std::array<int, 2>> path_vec;
        std::vector<int> temp_path;
        int cur = dst_fpga_id;
        temp_path.push_back(cur);

    
        while (cur != src_fpga_id && d[cur] != 1 << 30) {
          bool found = false;
          for (int i = h[cur]; ~i; i = ne[i]) {
            int v = e[i];
            if (d[v] + w[i] == d[cur]) {
              RG.edge_net_count[v][cur]++;
              cur = v;

              temp_path.push_back(cur);

              found = true;
              break;
            }
          }
          if (!found) break;
        }

        // 反转路径并记录
        std::reverse(temp_path.begin(), temp_path.end());
        for (int i = 0; i < temp_path.size(); i++) {
          path_vec.push_back({temp_path[i], i});
        }
        //debug
        // for(auto it:path_vec){
        //   std::cout<<it[0]<<" "<<it[1]<<" ";
        // } 
        // std::cout<<std::endl;

        RG.net_node_path_map[net_id][it] = path_vec;
      }
    }

    key.clear();
    for (int i = 1; i <= RG.Fpga_num; i++) {
      G[i].clear();
    }
  }

  calc_delay();
  net_delay_sort();
  print_design_route_out();

  // TODO
}
