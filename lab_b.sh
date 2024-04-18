#!/bin/bash

sed -i "s/pc:=.*.*.*.*.*.*/pc:=$1;/g" lab_part_b/heater.pha

./ph lab_part_b/heater.pha > lab_part_b/outputs/lab_b_$1.out

graph -T png -C -B --top-label almeida_pc_$1 -q 0.01 out_inv_heater -C -q 0.05 out_reachable_heater > lab_part_b/graphs/heater_graph_$1.png