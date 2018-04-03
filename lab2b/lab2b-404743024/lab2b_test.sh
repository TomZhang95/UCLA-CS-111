./lab2_list --iterations=1000 --threads=1 --sync=m >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=2 --sync=m >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=4 --sync=m >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=8 --sync=m >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=12 --sync=m >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=16 --sync=m >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=24 --sync=m >> lab2b_list.csv

./lab2_list --iterations=1000 --threads=1 --sync=s >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=2 --sync=s >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=4 --sync=s >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=8 --sync=s >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=12 --sync=s >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=16 --sync=s >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=24 --sync=s >> lab2b_list.csv

./lab2_list --threads=1 --iterations=1 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=1 --iterations=2 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=1 --iterations=4 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=1 --iterations=8 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=1 --iterations=16 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=4 --iterations=1 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=4 --iterations=2 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=4 --iterations=4 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=4 --iterations=8 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=4 --iterations=16 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=8 --iterations=1 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=8 --iterations=2 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=8 --iterations=4 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=8 --iterations=8 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=8 --iterations=16 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=12 --iterations=1 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=12 --iterations=2 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=12 --iterations=4 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=12 --iterations=8 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=12 --iterations=16 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=16 --iterations=1 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=16 --iterations=2 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=16 --iterations=4 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=16 --iterations=8 --lists=4 --yield=id >> lab2b_list.csv
./lab2_list --threads=16 --iterations=16 --lists=4 --yield=id >> lab2b_list.csv

./lab2_list --threads=1 --iterations=10 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=1 --iterations=20 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=1 --iterations=40 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=1 --iterations=80 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=4 --iterations=10 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=4 --iterations=20 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=4 --iterations=40 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=4 --iterations=80 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=8 --iterations=10 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=8 --iterations=20 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=8 --iterations=40 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=8 --iterations=80 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=12 --iterations=10 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=12 --iterations=20 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=12 --iterations=40 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=12 --iterations=80 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=16 --iterations=10 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=16 --iterations=20 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=16 --iterations=40 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=16 --iterations=80 --lists=4 --yield=id --sync=s >> lab2b_list.csv
./lab2_list --threads=1 --iterations=10 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=1 --iterations=20 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=1 --iterations=40 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=1 --iterations=80 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=4 --iterations=10 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=4 --iterations=20 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=4 --iterations=40 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=4 --iterations=80 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=8 --iterations=10 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=8 --iterations=20 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=8 --iterations=40 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=8 --iterations=80 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=12 --iterations=10 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=12 --iterations=20 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=12 --iterations=40 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=12 --iterations=80 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=16 --iterations=10 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=16 --iterations=20 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=16 --iterations=40 --lists=4 --yield=id --sync=m >> lab2b_list.csv
./lab2_list --threads=16 --iterations=80 --lists=4 --yield=id --sync=m >> lab2b_list.csv

#./lab2_list --iterations=1000 --threads=1 --lists=1 --sync=m >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=1 --lists=4 --sync=m >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=1 --lists=8 --sync=m >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=1 --lists=16 --sync=m >> lab2b_list.csv
#./lab2_list --iterations=1000 --threads=2 --lists=1 --sync=m >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=2 --lists=4 --sync=m >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=2 --lists=8 --sync=m >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=2 --lists=16 --sync=m >> lab2b_list.csv
#./lab2_list --iterations=1000 --threads=4 --lists=1 --sync=m >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=4 --lists=4 --sync=m >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=4 --lists=8 --sync=m >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=4 --lists=16 --sync=m >> lab2b_list.csv
#./lab2_list --iterations=1000 --threads=8 --lists=1 --sync=m >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=8 --lists=4 --sync=m >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=8 --lists=8 --sync=m >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=8 --lists=16 --sync=m >> lab2b_list.csv
#./lab2_list --iterations=1000 --threads=12 --lists=1 --sync=m >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=12 --lists=4 --sync=m >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=12 --lists=8 --sync=m >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=12 --lists=16 --sync=m >> lab2b_list.csv 

#./lab2_list --iterations=1000 --threads=1 --lists=1 --sync=s >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=1 --lists=4 --sync=s >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=1 --lists=8 --sync=s >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=1 --lists=16 --sync=s >> lab2b_list.csv
#./lab2_list --iterations=1000 --threads=2 --lists=1 --sync=s >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=2 --lists=4 --sync=s >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=2 --lists=8 --sync=s >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=2 --lists=16 --sync=s >> lab2b_list.csv
#./lab2_list --iterations=1000 --threads=4 --lists=1 --sync=s >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=4 --lists=4 --sync=s >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=4 --lists=8 --sync=s >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=4 --lists=16 --sync=s >> lab2b_list.csv
#./lab2_list --iterations=1000 --threads=8 --lists=1 --sync=s >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=8 --lists=4 --sync=s >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=8 --lists=8 --sync=s >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=8 --lists=16 --sync=s >> lab2b_list.csv
#./lab2_list --iterations=1000 --threads=12 --lists=1 --sync=s >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=12 --lists=4 --sync=s >> lab2b_list.csv
./lab2_list --iterations=1000 --threads=12 --lists=8 --sync=s >> lab2b_list.csv 
./lab2_list --iterations=1000 --threads=12 --lists=16 --sync=s >> lab2b_list.csv 
