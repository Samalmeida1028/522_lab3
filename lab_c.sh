#!/bin/bash

sed -i "s/pc:=.*.*.*.*.*.*/pc:=$1;/g" lab_part_c/car.pha

./ph lab_part_c/car.pha > lab_part_c/outputs/lab_c_$1.out

graph -T png -C -B --top-label almeida_pc_$1 -q 0.01 out_inv_car -C -q 0.05 out_reachable_car > lab_part_c/graphs/car_graph_$1.png