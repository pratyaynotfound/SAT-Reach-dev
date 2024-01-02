var=$(./SAT-Reach -m ../testcases/platoon/platoon_hybrid.xml -c ../testcases/platoon/platoon_hybrid.cfg --time-horizon 20 --time-step 0.01 --depth 1 -v t,x1 --CE first -F "loc=2 & 1*t<=20" -e bmc)
case "$var" in
    *"The model is UNSAFE"* ) echo "OK";;
    * ) exit 1;;
esac

var=$(./SAT-Reach  -m ../testcases/FlatFilteredOscillator/Oscillator.xml -c ../testcases/FlatFilteredOscillator/Oscillator.cfg --depth 3 --time-step 0.01 --time-horizon 10 -v p,q --CE first -F "loc=1 & 1*p<=0 & 1*q+0.714286*p>=0" -e bmc)
case "$var" in
    *"The model is UNSAFE"* ) echo "OK";;
    * ) exit 1;;
esac

var=$(./SAT-Reach -m ../testcases/two_tanks/two_tanks.xml -c ../testcases/two_tanks/two_tanks.cfg --time-horizon 10 --time-step 0.01 --depth 3 -o out.txt -v x1,x2 --CE first -F "loc=2 & 1*x2<=1" -e bmc)
case "$var" in
    *"The model is UNSAFE"* ) echo "OK";;
    * ) exit 1;;
esac

var=$(./SAT-Reach -m ../testcases/NAV_3_2.xml -c ../testcases/NAV_3_2.cfg --time-horizon 10 --time-step 0.01 --depth 6 -o out.txt -v x1,x2 --CE first -F "loc=7 & 1*x1>=2 & 1*x1<=3 & 1*x2>=0 & 1*x2<=1" -e bmc)
case "$var" in
    *"The model is UNSAFE"* ) echo "OK";;
    * ) exit 1;;
esac

var=$(./SAT-Reach -m ../testcases/two_tanks/two_tanks.xml -c ../testcases/two_tanks/two_tanks.cfg --time-horizon 10 --time-step 0.01 --depth 12 -o out.txt -v x1,x2 --CE first -F "loc=4 & 1*x1<=1 & 1*x2>=0" -e bmc)
case "$var" in
    *"The model is SAFE"* ) echo "OK";;
    * ) exit 1;;
esac

var=$(./SAT-Reach -m ../testcases/NAV_3_2.xml -c ../testcases/NAV_3_2.cfg --time-horizon 10 --time-step 0.01 --depth 10 -o out.txt -v x1,x2 --CE first -F "loc=9 & 1*x1>=2 & 1*x1<=3 & 1*x2>=2 & 1*x2<=3" -e bmc)
case "$var" in
    *"The model is SAFE"* ) echo "OK";;
    * ) exit 1;;
esac

var=$(./SAT-Reach -m ../testcases/NAV_3_3.xml -c ../testcases/NAV_3_3.cfg --time-horizon 10 --time-step 0.01 --depth 6 -o out.txt -v x1,x2 --CE first -F "loc=9 & 1*x1>=0 & 1*x1<=1 & 1*x2>=2 & 1*x2<=3 & 1*x3>=2 & 1*x3<=3" -e bmc)
case "$var" in
    *"The model is SAFE"* ) echo "OK";;
    * ) exit 1;;
esac

var=$(./SAT-Reach -m ../testcases/NAV_3_3.xml -c ../testcases/NAV_3_3.cfg --time-horizon 10 --time-step 0.01 --depth 6 -o out.txt -v x1,x2 --CE first -F "loc=15 & 1*x1>=1 & 1*x1<=2 & 1*x2>=1 & 1*x2<=2 & 1*x3>=2 & 1*x3<=3" -e bmc)
case "$var" in
    *"The model is SAFE"* ) echo "OK";;
    * ) exit 1;;
esac
