#!/bin/bash

graph -T png -C -B --top-label almeida_pc_$1 -q 0.01 lab_part_a/out_reachable/out_reachable_ball_0.500 -C -q 0.05 lab_part_a/out_reachable/out_reachable_ball_0.050 > lab_part_a/ball_graph_lab.png