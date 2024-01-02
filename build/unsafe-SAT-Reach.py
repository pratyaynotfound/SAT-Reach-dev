import os
import string
import re
from subprocess import Popen, PIPE


example=[]

#for platoon
example.append(['Platoon','runlim -s 4096 ./SAT-Reach -m ../testcases/platoon/platoon_hybrid.xml -c ../testcases/platoon/platoon_hybrid.cfg --time-horizon 20 --time-step 0.01 --depth 1 -v t,x1 --CE first -F "loc=2 & 1*t<=20" -e bmc'])

#for Oscillator and Filter_Oscillator
example.append(['Oscillator' , 'runlim -s 4096 ./SAT-Reach  -m ../testcases/FlatFilteredOscillator/Oscillator.xml -c ../testcases/FlatFilteredOscillator/Oscillator.cfg --depth 3 --time-step 0.01 --time-horizon 10 -v p,q --CE first -F "loc=1 & 1*p<=0 & 1*q+0.714286*p>=0" -e bmc'])
example.append(['F_oscillator_4' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/FlatFilteredOscillator/f_osc_4.xml -c ../testcases/FlatFilteredOscillator/f_osc_4.cfg --time-step 0.01 --time-horizon 10 --depth 3 -v p,q -o out.txt --CE first -F "loc=1 & 1*p<=0 & 1*q+0.714286*p>=0" -e bmc'])
example.append(['F_oscillator_8' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/FlatFilteredOscillator/f_osc_8.xml -c ../testcases/FlatFilteredOscillator/f_osc_8.cfg --time-step 0.01 --time-horizon 10 --depth 3 -v p,q -o out.txt --CE first -F "loc=1 & 1*p<=0 & 1*q+0.714286*p>=0" -e bmc'])
example.append(['F_oscillator_16' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/FlatFilteredOscillator/f_osc_16.xml -c ../testcases/FlatFilteredOscillator/f_osc_16.cfg --time-step 0.01 --time-horizon 10 --depth 3 -v p,q -o out.txt --CE first -F "loc=1 & 1*p<=0 & 1*q+0.714286*p>=0" -e bmc'])
example.append(['F_oscillator_32' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/FlatFilteredOscillator/f_osc_32.xml -c ../testcases/FlatFilteredOscillator/f_osc_32.cfg --time-step 0.01 --time-horizon 10 --depth 3 -v p,q -o out.txt --CE first -F "loc=1 & 1*p<=0 & 1*q+0.714286*p>=0" -e bmc'])
example.append(['F_oscillator_64' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/FlatFilteredOscillator/f_osc_64.xml -c ../testcases/FlatFilteredOscillator/f_osc_64.cfg --time-step 0.01 --time-horizon 10 --depth 3 -v p,q -o out.txt --CE first -F "loc=1 & 1*p<=0 & 1*q+0.714286*p>=0" -e bmc'])


#for Two_tank
example.append(['Two_tank_U1' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/two_tanks/two_tanks.xml -c ../testcases/two_tanks/two_tanks.cfg --time-horizon 10 --time-step 0.01 --depth 3 -o out.txt -v x1,x2 --CE first -F "loc=2 & 1*x2<=1" -e bmc'])


#for NAV_3_2
example.append(['NAV_U1','runlim -s 4096 ./SAT-Reach -m ../testcases/NAV_3_2.xml -c ../testcases/NAV_3_2.cfg --time-horizon 10 --time-step 0.01 --depth 6 -o out.txt -v x1,x2 --CE first -F "loc=7 & 1*x1>=2 & 1*x1<=3 & 1*x2>=0 & 1*x2<=1" -e bmc'])
example.append(['NAV_U2','runlim -s 4096 ./SAT-Reach -m ../testcases/NAV_3_2.xml -c ../testcases/NAV_3_2.cfg --time-horizon 10 --time-step 0.01 --depth 7 -o out.txt -v x1,x2 --CE first -F "loc=4 & 1*x1>=1 & 1*x1<=2 & 1*x2>=0 & 1*x2<=1" -e bmc'])

#for Nav3_inst1
example.append(['NAV_U3' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/Nav3_inst1.xml -c ../testcases/Nav3_inst1.cfg --time-horizon 10 --time-step 0.01 --depth 3 -o out.txt -v x1,x2 --CE first -F "loc=6 & 1*x1>=2 & 1*x1<=3 & 1*x2>=1 & 1*x2<=2" -e bmc'])
example.append(['NAV_U4' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/Nav3_inst1.xml -c ../testcases/Nav3_inst1.cfg --time-horizon 10 --time-step 0.01 --depth 2 -o out.txt -v x1,x2 --CE first -F "loc=5 & 1*x1>=1 & 1*x1<=2 & 1*x2>=1 & 1*x2<=2" -e bmc'])


#for NAV_3_3
example.append(['NAV_U5' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/NAV_3_3.xml -c ../testcases/NAV_3_3.cfg --time-horizon 10 --time-step 0.01 --depth 6 -o out.txt -v x1,x2 --CE first -F "loc=7 & 1*x1>=0 & 1*x1<=1 & 1*x2>=2 & 1*x2<=3 & 1*x3>=0 & 1*x3<=1" -e bmc'])
example.append(['NAV_U6' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/NAV_3_3.xml -c ../testcases/NAV_3_3.cfg --time-horizon 10 --time-step 0.01 --depth 5 -o out.txt -v x1,x2 --CE first -F "loc=8 & 1*x1>=0 & 1*x1<=1 & 1*x2>=2 & 1*x2<=3 & 1*x3>=1 & 1*x3<=2" -e bmc'])


#for NAV_3_4
example.append(['NAV_U7' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/NAV_3_4.xml -c ../testcases/NAV_3_4.cfg --time-horizon 10 --time-step 0.01 --depth 1 -o out.txt -v x1,x2 --CE first -F "loc=10 & 1*x1>=0 & 1*x1<=1 & 1*x2>=1 & 1*x2<=2 & 1*x3>=0 & 1*x3<=1 & 1*x4>=0 & 1*x4<=1" -e bmc'])
example.append(['NAV_U8' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/NAV_3_4.xml -c ../testcases/NAV_3_4.cfg --time-horizon 10 --time-step 0.01 --depth 8 -o out.txt -v x1,x2 --CE first -F "loc=41 & 1*x1>=1 & 1*x1<=2 & 1*x2>=1 & 1*x2<=2 & 1*x3>=1 & 1*x3<=2 & 1*x4>=1 & 1*x4<=2" -e bmc'])
example.append(['NAV_U9' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/NAV_3_4.xml -c ../testcases/NAV_3_4.cfg --time-horizon 10 --time-step 0.01 --depth 6 -o out.txt -v x1,x2 --CE first -F "loc=51 & 1*x1>=1 & 1*x1<=2 & 1*x2>=2 & 1*x2<=3 & 1*x3>=1 & 1*x3<=2 & 1*x4>=2 & 1*x4<=3" -e bmc'])
example.append(['NAV_U10' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/NAV_3_4.xml -c ../testcases/NAV_3_4.cfg --time-horizon 10 --time-step 0.01 --depth 12 -o out.txt -v x1,x2 --CE first -F "loc=61 & 1*x1>=2 & 1*x1<=3 & 1*x2>=0 & 1*x2<=1 & 1*x3>=2 & 1*x3<=3 & 1*x4>=0 & 1*x4<=1" -e bmc'])
example.append(['NAV_U11' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/NAV_3_4.xml -c ../testcases/NAV_3_4.cfg --time-horizon 10 --time-step 0.01 --depth 9 -o out.txt -v x1,x2 --CE first -F "loc=68 & 1*x1>=2 & 1*x1<=3 & 1*x2>=1 & 1*x2<=2 & 1*x3>=1 & 1*x3<=2 & 1*x4>=1 & 1*x4<=2" -e bmc'])
example.append(['NAV_U12' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/NAV_3_4.xml -c ../testcases/NAV_3_4.cfg --time-horizon 10 --time-step 0.01 --depth 11 -o out.txt -v x1,x2 --CE first -F "loc=70 & 1*x1>=2 & 1*x1<=3 & 1*x2>=1 & 1*x2<=2 & 1*x3>=2 & 1*x3<=3 & 1*x4>=0 & 1*x4<=1" -e bmc'])
example.append(['NAV_U13' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/NAV_3_4.xml -c ../testcases/NAV_3_4.cfg --time-horizon 10 --time-step 0.01 --depth 10 -o out.txt -v x1,x2 --CE first -F "loc=71 & 1*x1>=2 & 1*x1<=3 & 1*x2>=1 & 1*x2<=2 & 1*x3>=2 & 1*x3<=3 & 1*x4>=1 & 1*x4<=2" -e bmc'])


#forn NAV30
example.append(['NAV_U14' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/30.xml -c ../testcases/30.cfg --time-horizon 10 --time-step 0.01 --depth 2 -o out.txt -v x1,x2 --CE first -F "loc=532 & 1*x1>=6 & 1*x1<=7 & 1*x2>=21 & 1*x2<=22" -e bmc'])
example.append(['NAV_U15' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/30.xml -c ../testcases/30.cfg --time-horizon 10 --time-step 0.01 --depth 12 -o out.txt -v x1,x2 --CE first -F "loc=412 & 1*x1>=11 & 1*x1<=12 & 1*x2>=16 & 1*x2<=17" -e bmc'])
example.append(['NAV_U16' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/30.xml -c ../testcases/30.cfg --time-horizon 10 --time-step 0.01 --depth 9 -o out.txt -v x1,x2 --CE first -F "loc=435 & 1*x1>=9 & 1*x1<=10 & 1*x2>=17 & 1*x2<=18" -e bmc'])
example.append(['NAV_U17' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/30.xml -c ../testcases/30.cfg --time-horizon 10 --time-step 0.01 --depth 10 -o out.txt -v x1,x2 --CE first -F "loc=436 & 1*x1>=10 & 1*x1<=11 & 1*x2>=17 & 1*x2<=18" -e bmc'])


#for Nav07
example.append(['NAV_U18' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/nav07.xml -c ../testcases/nav07.cfg --time-horizon 10 --time-step 0.01 --depth 10 -o out.txt -v x1,x2 --CE first -F "loc=208 & 1*x1>=4 & 1*x1<=5 & 1*x2>=22 & 1*x2<=23 & 1*v11>=-umax & 1*v11<=umax & 1*v22>=-umax & 1*v22<=umax" -e bmc'])
example.append(['NAV_U19' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/nav07.xml -c ../testcases/nav07.cfg --time-horizon 10 --time-step 0.01 --depth 12 -o out.txt -v x1,x2 --CE first -F "loc=199 & 1*x1>=3 & 1*x1<=4 & 1*x2>=21 & 1*x2<=22 & 1*v11>=-umax & 1*v11<=umax & 1*v22>=-umax & 1*v22<=umax" -e bmc'])
example.append(['NAV_U20' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/nav07.xml -c ../testcases/nav07.cfg --time-horizon 10 --time-step 0.01 --depth 13 -o out.txt -v x1,x2 --CE first -F "loc=193 & 1*x1>=3 & 1*x1<=4 & 1*x2>=20 & 1*x2<=21 & 1*v11>=-umax & 1*v11<=umax & 1*v22>=-umax & 1*v22<=umax" -e bmc'])
example.append(['NAV_U21' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/nav07.xml -c ../testcases/nav07.cfg --time-horizon 10 --time-step 0.01 --depth 15 -o out.txt -v x1,x2 --CE first -F "loc=181 & 1*x1>=3 & 1*x1<=4 & 1*x2>=18 & 1*x2<=19 & 1*v11>=-umax & 1*v11<=umax & 1*v22>=-umax & 1*v22<=umax" -e bmc'])
example.append(['NAV_U22' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/nav07.xml -c ../testcases/nav07.cfg --time-horizon 10 --time-step 0.01 --depth 17 -o out.txt -v x1,x2 --CE first -F "loc=169 & 1*x1>=3 & 1*x1<=4 & 1*x2>=16 & 1*x2<=17 & 1*v11>=-umax & 1*v11<=umax & 1*v22>=-umax & 1*v22<=umax" -e bmc'])
example.append(['NAV_U23' , 'runlim -s 4096 ./SAT-Reach -m ../testcases/nav07.xml -c ../testcases/nav07.cfg --time-horizon 10 --time-step 0.01 --depth 18 -o out.txt -v x1,x2 --CE first -F "loc=163 & 1*x1>=3 & 1*x1<=4 & 1*x2>=15 & 1*x2<=16 & 1*v11>=-umax & 1*v11<=umax & 1*v22>=-umax & 1*v22<=umax" -e bmc'])

#for parallel edges of NAV_3_2
example.append(['NAV_U1_P*','runlim -s 4096 ./SAT-Reach -m ../testcases/NAV_3_2_P.xml -c ../testcases/NAV_3_2_P.cfg --time-horizon 10 --time-step 0.01 --depth 6 -o out.txt -v x1,x2 --CE first -F "loc=7 & 1*x1>=2 & 1*x1<=3 & 1*x2>=0 & 1*x2<=1" -e bmc'])



if not os.path.isdir('unsafe_SATReach'):
	os.mkdir('unsafe_SATReach')
with open("./unsafe_SATReach/unsafe-SATReach.csv","w") as f:
	f.write("benchmark, Depth, Concrete_CE, abstract_CE, ra_time, opt_time, fal_time, #Paths_visited , #New_Paths, #postC\n")
	for i in range(0,len(example)):
		string_cmd = example[i][1]
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
			# Extracting time to search ce
			opt_time = "Time to search concrete counter-example(s) from the abstract path(s) (user time in seconds):"
			opt_time_str = output.find(opt_time)
			opt_time_start = int(opt_time_str) + len(opt_time) + 0
			opt_time_end = int(opt_time_str) + len(opt_time) + 8
			ce_opt_time = output[opt_time_start:opt_time_end];
			ce_opt_time = re.sub("[^\d\.]", "", ce_opt_time)
			#ce_opt_time = (float(ce_opt_time)/1000)
			#print(ce_opt_time)
			# Extracting Falsification time
			fal_time = "User Time  (in Seconds) ="
			fal_str = output.find(fal_time);
			fal_str_start = int(fal_str) + len(fal_time) + 1
			fal_str_end = int(fal_str) + len(fal_time) + 8
			ce_fal_time = output[fal_str_start:fal_str_end];
			ce_fal_time = re.sub("[^\d\.]", "", ce_fal_time)
			ra_time = float(ce_fal_time) - float(ce_opt_time) 
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

			#capture the depth
			bound =""
			if(int(ce_res)==1):
				depth_match = "Inside concrete trace plotter, no. of trajectory segs:"
				depth_str = output.find(depth_match)
				depth_start = int(depth_str) + len(depth_match) + 0
				depth_end = int(depth_str) + len(depth_match) + 8
				bound = output[depth_start:depth_end]
				bound = re.sub("[^\d\.]", "", bound)
				bound = int(bound) - 1
			else:
				bound = ""
			

			f.write(example[i][0] + ', '+ str(bound) + ', ' + ce_res + ', ' + ce_att + ', ' + str(ra_time) +', '+ str(ce_opt_time) + ', ' + str(ce_fal_time) + ', ' + path_num + ','+ path_num_new + ','+ postc_num + '\n')
			if not os.path.isdir("./unsafe_SATReach/"+ example[i][0]):
				os.mkdir("./unsafe_SATReach/"+ example[i][0])
			os.system("mv bad_poly unsafe_SATReach/" + example[i][0] +"/")
			if os.path.isfile("bad_trace.o"):
				os.system("mv bad_trace.o unsafe_SATReach/" + example[i][0] +"/")
			os.system("mv init_poly unsafe_SATReach/" + example[i][0] +"/")
			os.system("mv out.txt unsafe_SATReach/" + example[i][0] +"/")

		else:
			f.write(example[i][0] + ', '+ str("-") + ', ' + str("-") + ', ' + str("-") + ', ' + str("-") +', '+ str("-") + ', ' + str("-") + ', ' + str("-") + ', ' + ',' + ',' + '\n')


f.close()        
os.chdir('..')
print("results file of SAT-Reach created successfully :)\n")

