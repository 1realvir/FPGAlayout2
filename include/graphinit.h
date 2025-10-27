#pragma once
#include "_readdata.h"

constexpr int V=65; 
constexpr int E=2021; 

//初始化数据
void Init_Graph();

//进行建图
void Add_Edge(int u, int v, int c);

void dijkstra(int s);

void solve();

void printedge(int u, int v);

void print_path(int net_id);

void calc_delay(int net_id);

void printnet_path(int net_id);

int Find(int x);

void fenpei();
