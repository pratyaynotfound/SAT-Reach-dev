import os
import os.path
import time
from unittest import result

timelimit = 1000
runlim	= '''runlim -s 4096 -t ''' +str(timelimit)+''' -r '''+str(timelimit)
bach	= ''' ../bach -t -p -v2 -solver cmsat '''
resultdir = "result_mixed/"
opt_comp = "-semantics mixed "
opt_single = ""

def mkdir(path):
	folder = os.path.exists(path)
	if not folder:
		os.makedirs(path)
		print("new folder "+path)

def getResult(filename):
	checkresult = "unknown"
	for line in open(filename,"r"):
		if "REACHABLE\n" == line or "The target is reachable\n" == line :
			checkresult = "unsafe"
		elif "UNREACHABLE\n" == line or "The target is not reachable\n" == line:
			checkresult = "safe"
	return checkresult


def test(model_dir, model_name, spec_name, bound, opt):
	runlimdir = resultdir+model_dir
	runlimlog = runlimdir+model_name+"_"+spec_name+"_"+str(bound)+".runlim"
	logdir = runlimdir+'''log/'''
	logname = logdir+model_name+"_"+spec_name+"_"+str(bound)+".runlim"
	model_path = model_dir + model_name + ".xml"
	spec_path = model_dir + spec_name + ".cfg"
	mkdir(runlimdir)
	mkdir(logdir)
	cmd = runlim+" -o "+runlimlog+bach+" "+opt+" "+model_path+" "+spec_path+" "+str(bound)+" > "+logname
	print(cmd)
	t = time.time()
	rtn = os.system(cmd)
	t = time.time() - t
	with open(resultdir+"result.txt","a") as f:
		f.write("BACH-mixed")
		f.write(",")
		f.write(model_dir + model_name)
		f.write(",")
		f.write(model_dir + spec_name)
		f.write(",")
		f.write(getResult(logname))
		f.write(",")
		f.write(str(t))
		f.write(",")
		f.write(str(bound))
		f.write("\n")


mkdir(resultdir)
with open(resultdir+"result.txt","a") as f:
	f.truncate(0)
for i in [5,6,7,8]:
	test("AdaptiveCruiseController/safe/", "ACC"+str(i), "ACC"+str(i), 30, opt_comp)
for i in [5,6,7,8]:
	test("AdaptiveCruiseController/unsafe/", "ACC"+str(i), "ACC"+str(i), 30, opt_comp)
for i in [2,3,4,5]:
	test("DistributedController/", "DISC"+str(i), "DISC"+str(i), 30, opt_comp)
for i in [4,5,6]:
	test("Fischer/", "FISCS0"+str(i)+"-UBD0"+str(i), "F"+str(i), 20, opt_comp)
for i in [4,5,6]:
	test("Fischer/", "FISCU0"+str(i)+"-UBD0"+str(i), "F"+str(i), 30, opt_comp)
for i in [2,3,4]:
	test("Navigation/", "NAV_3_"+str(i), "NAV_3_"+str(i), 30, opt_single)
for i in [5,7,9]:
	test("TTEthernet/", "TTE"+str(i), "TTE"+str(i), 30, opt_comp)
for i in [1,2,3,4]:
	test("DutchRailwayNetwork/", "DRN_new", "BDR0"+str(i), 30, opt_comp)
