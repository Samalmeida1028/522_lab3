#!/bin/bash

sed -i "s/pc:=.*.*.*.*.*.*/pc:=$1;/g" lab_part_b/heater.pha

sed -i "s/inv.print(\"lab_part_b\/out_inv\/out_inv_heater.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*/inv.print(\"lab_part_b\/out_inv\/out_inv_heater_$1\",2);/g" lab_part_b/heater.pha


sed -i "s/reg.print(\"lab_part_b\/out_reachable\/out_reachable_heater.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*/reg.print(\"lab_part_b\/out_reachable\/out_reachable_heater_$1\",2);/g" lab_part_b/heater.pha

./ph lab_part_b/heater.pha > lab_part_b/outputs/lab_b_$1.out

echo FOR PC: $1 >> lab_part_b/results.txt
echo `grep cond1 lab_part_b/outputs/lab_b_$1.out` >> lab_part_b/results.txt

graph -T png -C -B --top-label almeida_pc_$1 -q 0.01 lab_part_b/out_inv/out_inv_heater_$1 -C -q 0.05 lab_part_b/out_reachable/out_reachable_heater_$1 > lab_part_b/graphs/heater_graph_$1.png
