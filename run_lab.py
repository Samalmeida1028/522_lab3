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


def run_labs(num):
    # run_lab_a(num)
    # run_lab_b(num)
    run_lab_c(num)




def main(args):
    processes = []
    if(len(args)<0):
        # run_lab_a(args[0])
        # run_lab_b(args[0])
        run_lab_c(args[0])
    else:
        for i in range(3000):
            if i%10==0:
                lab_process = threading.Thread(target=run_labs,args=[3-(i/1000)])
                lab_process.run()





if __name__ == "__main__":
    main(sys.argv[1:])
