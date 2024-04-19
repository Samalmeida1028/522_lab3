#!/bin/bash

graph -T png -C -B --top-label almeida_pc_$1 -q 0.01 lab_part_b/reachable_inv/out_reachable_heater_0.5 -C -q 0.05 lab_part_b/out_reachable/out_reachable_heater_0.05 > lab_part_b/heater_graph_lab.png