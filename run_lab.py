import threading
import subprocess
import time
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
    run_lab_a(num)
    run_lab_b(num)
    run_lab_c(num*10)



for i in range(5,1000):
    # lab_a_thread = threading.Thread(target=run_lab_a, args=[1-(float(i)/float(1000))])
    # lab_a_thread.run()
    # lab_b_thread = threading.Thread(target=run_lab_b, args=[1-(float(i)/float(1000))])
    # lab_b_thread.run()
    # lab_c_thread = threading.Thread(target=run_lab_c, args=[1-(float(i)/float(100))])
    # lab_c_thread.run()
    lab_thread = threading.Thread(target=run_labs,args=[1-i/1000])
    lab_thread.run()




