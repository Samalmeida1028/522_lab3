#!/bin/bash

sed -i "s/pc:=.*.*.*.*.*.*/pc:=$1;/g" lab_part_a/bouncing_ball.pha
sed -i "s/inv.print(\"lab_part_a\/out_inv\/out_inv_ball.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*/inv.print(\"lab_part_a\/out_inv\/out_inv_ball_$1\",2);/g" lab_part_a/bouncing_ball.pha
sed -i "s/reach.print(\"lab_part_a\/out_reachable\/out_reachable_ball.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*/reach.print(\"lab_part_a\/out_reachable\/out_reachable_ball_$1\",2);/g" lab_part_a/bouncing_ball.pha


./ph lab_part_a/bouncing_ball.pha > lab_part_a/outputs/lab_a_$1.out

graph -T png -C -B --top-label almeida_pc_$1 -q 0.01 lab_part_a/out_inv/out_inv_ball_$1 -C -q 0.05 lab_part_a/out_reachable/out_reachable_ball_$1 > lab_part_a/graphs/lab_a_$1.png