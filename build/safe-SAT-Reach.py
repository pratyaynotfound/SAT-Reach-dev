import os
import string
import re
from subprocess import Popen, PIPE


example=[]

#Two_tanks
example.append(['Two_tank_S1' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/two_tanks/two_tanks.xml -c ../testcases/two_tanks/two_tanks.cfg --time-horizon 10 --time-step 0.01 --depth 12 -o out.txt -v x1,x2 --CE first -F "loc=4 & 1*x1<=1 & 1*x2>=0" -e bmc'])

#for NAV_3_2
example.append(['NAV_S1','runlim -s 4096 ./SAT-Reach -m ../testcases/NAV_3_2.xml -c ../testcases/NAV_3_2.cfg --time-horizon 10 --time-step 0.01 --depth 10 -o out.txt -v x1,x2 --CE first -F "loc=9 & 1*x1>=2 & 1*x1<=3 & 1*x2>=2 & 1*x2<=3" -e bmc'])

#for ACC03
example.append(['ACCS03_1' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/ACCS03/ACCS03.xml -c ../testcases/ACCS03/ACCS03.cfg --time-horizon 5 --time-step 0.1 --depth 7 -o out.txt -v x1,x2 --CE first -F "loc=8 & 1*autd201 <= 3" -e bmc'])
example.append(['ACCS03_2' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/ACCS03/ACCS03.xml -c ../testcases/ACCS03/ACCS03.cfg --time-horizon 5 --time-step 0.1 --depth 7 -o out.txt -v x1,x2 --CE first -F "loc=9 & 1*autd101 <= 3" -e bmc'])

#for NAV_3_3
example.append(['NAV_S2','runlim -s 4096 ./SAT-Reach -m ../testcases/NAV_3_3.xml -c ../testcases/NAV_3_3.cfg --time-horizon 10 --time-step 0.01 --depth 6 -o out.txt -v x1,x2 --CE first -F "loc=9 & 1*x1>=0 & 1*x1<=1 & 1*x2>=2 & 1*x2<=3 & 1*x3>=2 & 1*x3<=3" -e bmc'])
example.append(['NAV_S3','runlim -s 4096 ./SAT-Reach -m ../testcases/NAV_3_3.xml -c ../testcases/NAV_3_3.cfg --time-horizon 10 --time-step 0.01 --depth 6 -o out.txt -v x1,x2 --CE first -F "loc=15 & 1*x1>=1 & 1*x1<=2 & 1*x2>=1 & 1*x2<=2 & 1*x3>=2 & 1*x3<=3" -e bmc'])
example.append(['NAV_S4','runlim -s 4096 ./SAT-Reach -m ../testcases/NAV_3_3.xml -c ../testcases/NAV_3_3.cfg --time-horizon 10 --time-step 0.01 --depth 19 -o out.txt -v x1,x2 --CE first -F "loc=23 & 1*x1>=1 & 1*x1<=2 & 1*x2>=1 & 1*x2<=2 & 1*x3>=1 & 1*x3<=2" -e bmc'])
example.append(['NAV_S5','runlim -s 4096 ./SAT-Reach -m ../testcases/NAV_3_3.xml -c ../testcases/NAV_3_3.cfg --time-horizon 10 --time-step 0.01 --depth 19 -o out.txt -v x1,x2 --CE first -F "loc=27 & 1*x1>=1 & 1*x1<=2 & 1*x2>=2 & 1*x2<=3 & 1*x3>=2 & 1*x3<=3" -e bmc'])

#for NAV_3_4
example.append(['NAV_S6','runlim -s 4096 ./SAT-Reach -m ../testcases/NAV_3_4.xml -c ../testcases/NAV_3_4.cfg --time-horizon 10 --time-step 0.01 --depth 15 -o out.txt -v x1,x2 --CE first -F "loc=5 & 1*x1>=0 & 1*x1<=1 & 1*x2>=0 & 1*x2<=1 & 1*x3>=1 & 1*x3<=2 & 1*x4>=1 & 1*x4<=2" -e bmc'])
example.append(['NAV_S7','runlim -s 4096 ./SAT-Reach -m ../testcases/NAV_3_4.xml -c ../testcases/NAV_3_4.cfg --time-horizon 10 --time-step 0.01 --depth 12 -o out.txt -v x1,x2 --CE first -F "loc=8 & 1*x1>=0 & 1*x1<=1 & 1*x2>=0 & 1*x2<=1 & 1*x3>=2 & 1*x3<=3 & 1*x4>=1 & 1*x4<=2" -e bmc'])
example.append(['NAV_S8','runlim -s 4096 ./SAT-Reach -m ../testcases/NAV_3_4.xml -c ../testcases/NAV_3_4.cfg --time-horizon 10 --time-step 0.01 --depth 15 -o out.txt -v x1,x2 --CE first -F "loc=27 & 1*x1>=0 & 1*x1<=1 & 1*x2>=2 & 1*x2<=3 & 1*x3>=2 & 1*x3<=3 & 1*x4>=2 & 1*x4<=3" -e bmc'])
example.append(['NAV_S9','runlim -s 4096 ./SAT-Reach -m ../testcases/NAV_3_4.xml -c ../testcases/NAV_3_4.cfg --time-horizon 10 --time-step 0.01 --depth 14 -o out.txt -v x1,x2 --CE first -F "loc=46 & 1*x1>=1 & 1*x1<=2 & 1*x2>=2 & 1*x2<=3 & 1*x3>=0 & 1*x3<=1 & 1*x4>=0 & 1*x4<=1" -e bmc'])
example.append(['NAV_S10','runlim -s 4096 ./SAT-Reach -m ../testcases/NAV_3_4.xml -c ../testcases/NAV_3_4.cfg --time-horizon 10 --time-step 0.01 --depth 14 -o out.txt -v x1,x2 --CE first -F "loc=80 & 1*x1>=2 & 1*x1<=3 & 1*x2>=2 & 1*x2<=3 & 1*x3>=2 & 1*x3<=3 & 1*x4>=1 & 1*x4<=2" -e bmc'])
example.append(['NAV_S11','runlim -s 4096 ./SAT-Reach -m ../testcases/NAV_3_4.xml -c ../testcases/NAV_3_4.cfg --time-horizon 10 --time-step 0.01 --depth 15 -o out.txt -v x1,x2 --CE first -F "loc=81 & 1*x1>=2 & 1*x1<=3 & 1*x2>=2 & 1*x2<=3 & 1*x3>=2 & 1*x3<=3 & 1*x4>=2 & 1*x4<=3" -e bmc'])

#for ACC05
example.append(['ACCS05_1' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/ACCS05/ACCS05.xml -c ../testcases/ACCS05/ACCS05.cfg --time-horizon 5 --time-step 0.1 --depth 4 -o out.txt -v x1,x2 --CE first -F "loc=9 & 1*autd101 >= 1 & 1*autd201 >= 1" -e bmc'])
example.append(['ACCS05_2' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/ACCS05/ACCS05.xml -c ../testcases/ACCS05/ACCS05.cfg --time-horizon 5 --time-step 0.1 --depth 4 -o out.txt -v x1,x2 --CE first -F "loc=16 & 1*autd101 >= 1 & 1*autd201 <= 3 & 1*autd401 >= 1" -e bmc'])


#for NAV_30
example.append(['NAV_S12' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/30.xml -c ../testcases/30.cfg --time-horizon 10 --time-step 0.01 --depth 15 -o out.txt -v x1,x2 --CE first -F "loc=516 & 1*x1>=15 & 1*x1<=16 & 1*x2>=20 & 1*x2<=21" -e bmc'])
example.append(['NAV_S13' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/30.xml -c ../testcases/30.cfg --time-horizon 10 --time-step 0.01 --depth 15 -o out.txt -v x1,x2 --CE first -F "loc=541 & 1*x1>=15 & 1*x1<=16 & 1*x2>=21 & 1*x2<=22" -e bmc'])
example.append(['NAV_S14' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/30.xml -c ../testcases/30.cfg --time-horizon 10 --time-step 0.01 --depth 20 -o out.txt -v x1,x2 --CE first -F "loc=547 & 1*x1>=21 & 1*x1<=22 & 1*x2>=21 & 1*x2<=22" -e bmc'])

#for NAV_07
example.append(['NAV_S15' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/nav07.xml -c ../testcases/nav07.cfg --time-horizon 10 --time-step 0.01 --depth 16 -o out.txt -v x1,x2 --CE first -F "loc=176 & 1*x1>=5 & 1*x1<=6 & 1*x2>=17 & 1*x2<=18 & 1*v11>=-umax & 1*v11<=umax & 1*v22>=-umax & 1*v22<=umax" -e bmc'])
example.append(['NAV_S16' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/nav07.xml -c ../testcases/nav07.cfg --time-horizon 10 --time-step 0.01 --depth 10 -o out.txt -v x1,x2 --CE first -F "loc=166 & 1*x1>=13 & 1*x1<=14 & 1*x2>=15 & 1*x2<=16 & 1*v11>=-umax & 1*v11<=umax & 1*v22>=-umax & 1*v22<=umax" -e bmc'])




if not os.path.isdir('safe_SATReach'):
	os.mkdir('safe_SATReach')
with open("./safe_SATReach/safe_SATReach.csv","w") as f:
	f.write("Benchmark, Depth, Concrete_CE, Abstract_CE, Time, #Paths_visited , #New_Paths, #postC\n")
	for i in range(0,len(example)):
		string_cmd = example[i][1]
		temp=string_cmd.split()
		print('\nrunning '+ example[i][0])
		p = Popen(string_cmd, stdout=PIPE, stderr=PIPE, stdin=PIPE, shell=True)
		output = str(p.communicate())
		#print(output)
		res = "Number of concrete ce trajectories found:"
		res_str = output.find(res)
		res_start = int(res_str)+len(res)+1
		res_end = int(res_str)+len(res)+8
		#print(res_str)
		if(res_str>=0):
			ce_res = output[res_start:res_end];
			ce_res = re.sub("[^\d\.]", "", ce_res)
			#print(ce_res)
			# Extracting the #att
			att = "number of abstract ce-paths found for exploration:"		
			att_str = output.find(att)
			att_start = int(att_str)+len(att)+1
			att_end = int(att_str)+len(att)+8
			ce_att = output[att_start:att_end];
			ce_att = re.sub("[^\d\.]", "", ce_att)
			#print(ce_att)
			# Extracting Falsification time
			fal_time = "User Time  (in Seconds) ="
			fal_str = output.find(fal_time);
			fal_str_start = int(fal_str) + len(fal_time) + 1
			fal_str_end = int(fal_str) + len(fal_time) + 8
			ce_fal_time = output[fal_str_start:fal_str_end];
			ce_fal_time = re.sub("[^\d\.]", "", ce_fal_time)
			#ra_time = float(ce_fal_time) - float(ce_opt_time) 
			# Extracting total paths 
			path = "Total paths visited by our old Algorithm:"
			path_str = output.find(path)
			path_start = int(path_str) + len(path) + 0
			path_end = int(path_str) + len(path) + 8
			path_num = output[path_start:path_end];
			path_num = re.sub("[^\d\.]", "", path_num)
			# Extracting total postC
			path = "Number of postC operation done:"
			path_str = output.find(path)
			path_start = int(path_str) + len(path) + 0
			path_end = int(path_str) + len(path) + 8
			postc_num = output[path_start:path_end];
			postc_num = re.sub("[^\d\.]", "", postc_num)

			# Extracting total runs 
			path = "Total paths visited by our new Algorithm:"
			path_str = output.find(path)
			path_start = int(path_str) + len(path) + 0
			path_end = int(path_str) + len(path) + 8
			path_num_new = output[path_start:path_end];
			path_num_new = re.sub("[^\d\.]", "", path_num_new)
			bound = temp[13]
			#print(bound)
			f.write(example[i][0] + ', '+ str(bound) + ', ' + ce_res + ', ' + ce_att + ', '  + str(ce_fal_time) + ', ' + path_num + ','+ path_num_new + ','+ postc_num + '\n')
			if not os.path.isdir("./safe_SATReach/"+ example[i][0]):
				os.mkdir("./safe_SATReach/"+ example[i][0])
			os.system("mv bad_poly safe_SATReach/" + example[i][0] +"/")
			if os.path.isfile("bad_trace.o"):
				os.system("mv bad_trace.o safe_SATReach/" + example[i][0] +"/")
			os.system("mv init_poly safe_SATReach/" + example[i][0] +"/")
			os.system("mv out.txt safe_SATReach/" + example[i][0] +"/")

		else:
			f.write(example[i][0] + ', '+ str("-") + ', ' + str("-") + ', ' + str("-") + ', ' + str("-") + ', ' + str("-") + ', ' + str("-") + ', ' + str("-") + ', ' + '\n')


f.close()        
os.chdir('..')
print("results file of XSpeed created successfully :)\n")

