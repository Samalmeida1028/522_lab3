import threading
import subprocess
import time
import sys
import multiprocessing




def run_lab_a(num):
    start = time.time()
    print("Lab A PC= ",num)
    args = ['./lab_a.sh',f"{num:.3f}"]
    subprocess.run(args)
    print(f"Lab A with PC= {num} ran in {time.time()-start} seconds")

def run_lab_b(num):
    start = time.time()
    print("Lab B PC= ",num)
    args = ['./lab_b.sh',f"{num:.3f}"]
    subprocess.run(args)
    print(f"Lab B with PC= {num} ran in {time.time()-start} seconds")

def run_lab_c(num):
    start = time.time()
    print("Lab C PC= ",num)
    args = ['./lab_c.sh',f"{num:.3f}"]
    subprocess.run(args)
    print(f"Lab C with PC= {num} ran in {time.time()-start} seconds")


def run_labs(num,target):
    # print(target)
    if('a' in target):
        run_lab_a(num)
    if('b' in target):
        run_lab_b(num)
    if('c' in target):
        run_lab_c(num)




def main(args):
    processes = []
    print(args)
    subprocess.run("./clear_results.sh")
    for i in range(int(args[0]),int(args[1])):
        lab_process = threading.Thread(target=run_labs,args=[float(args[2])-(i/float(args[1])),args[3]])
        lab_process.run()
    if 'a' in args[3]:
        subprocess.run("./graph_part_a.sh")





if __name__ == "__main__":
    print(sys.argv[1:])
    main(sys.argv[1:])
