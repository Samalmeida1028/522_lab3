#!/bin/bash

sed -i "s/pc:=.*.*.*.*.*.*/pc:=$1;/g" lab_part_a/bouncing_ball.pha

./ph lab_part_a/bouncing_ball.pha > lab_part_a/outputs/lab_a_$1.out

graph -T png -C -B --top-label almeida_pc_$1 -q 0.01 out_inv_ball -C -q 0.05 out_reachable_ball > lab_part_a/graphs/lab_a_$1.png