#!/bin/bash

sed -i "s/pc:=.*.*.*.*.*.*/pc:=$1;/g" lab_part_c/car.pha
sed -i "s/inv\.print(\"lab_part_c\/out_inv\/out_inv_car.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*/inv\.print(\"lab_part_c\/out_inv\/out_inv_car_$1\",2);/g" lab_part_c/car.pha
sed -i "s/reach\.print(\"lab_part_c\/out_reachable\/out_reachable_car.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*/reach\.print(\"lab_part_c\/out_reachable\/out_reachable_car_$1\",2);/g" lab_part_c/car.pha

./ph lab_part_c/car.pha > lab_part_c/outputs/lab_c_$1.out


echo FOR PC: $1 >> lab_part_c/results.txt
echo `grep cond1 lab_part_c/outputs/lab_c_$1.out` >> lab_part_c/results.txt
echo `grep cond2 lab_part_c/outputs/lab_c_$1.out` >> lab_part_c/results.txt
echo `grep cond3 lab_part_c/outputs/lab_c_$1.out` >> lab_part_c/results.txt

graph -T png -C -B --top-label almeida_pc_$1 -q 0.01 lab_part_c/out_inv/out_inv_car_$1 -C -q 0.05 lab_part_c/out_reachable/out_reachable_car_$1 > lab_part_c/graphs/car_graph_$1.png