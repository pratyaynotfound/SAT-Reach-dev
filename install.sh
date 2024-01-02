# install prerequisites

sudo apt -y update && sudo apt -y upgrade
sudo apt -y install git build-essential libssl-dev wget cmake python3 lzip libgmp3-dev m4 libcairo2-dev libpango1.0-dev runlim libglpk40

# install openjdk16

sudo apt -y install openjdk-16-jre-headless

# install z3

sudo wget https://github.com/Z3Prover/z3/archive/refs/tags/z3-4.8.10.tar.gz
tar -xvf z3-4.8.10.tar.gz
cd ./z3-z3-4.8.10/
sudo mkdir build && \
	cd build && \
	sudo cmake -G "Unix Makefiles" ../ && \
	sudo make -j4
sudo make install
cd ..

# install GLPK

cd ..
sudo wget https://ftp.gnu.org/gnu/glpk/glpk-4.35.tar.gz
tar -xvf glpk-4.35.tar.gz
cd ./glpk-4.35
sudo ./configure && \
	sudo make all && \
	sudo make install

# install GMP

cd ..
sudo wget https://gmplib.org/download/gmp/gmp-6.2.1.tar.lz
tar -xvf gmp-6.2.1.tar.lz
cd ./gmp-6.2.1
sudo ./configure && \
	sudo make all && \
	sudo make install

# install NLOpt

cd ..
wget https://github.com/stevengj/nlopt/archive/v2.6.2.tar.gz
tar -xvf v2.6.2.tar.gz
cd ./nlopt-2.6.2
sudo cmake . && sudo make && sudo make install

# install Boost

cd ..
sudo wget https://sourceforge.net/projects/boost/files/boost/1.65.1/boost_1_65_1.tar.gz/download
sudo mv download boost_1_65_1.tar.gz
tar -xvf boost_1_65_1.tar.gz
cd ./boost_1_65_1
sudo ./bootstrap.sh && \
	sudo ./b2 install

# install SUNDIALS

cd ..
sudo wget https://github.com/LLNL/sundials/releases/download/v5.6.1/sundials-5.6.1.tar.gz
tar -xvf sundials-5.6.1.tar.gz
cd ./sundials-5.6.1/
sudo mkdir instdir builddir
cd ./builddir
sudo cmake .. && sudo make && sudo make install
cd ..

# install GNUPlot

cd ..
sudo wget https://sourceforge.net/projects/gnuplot/files/gnuplot/5.0.1/gnuplot-5.0.1.tar.gz/download
sudo mv download gnuplot-5.0.1.tar.gz
tar -xvf gnuplot-5.0.1.tar.gz
cd ./gnuplot-5.0.1
sudo ./configure && \
	sudo make && \
	sudo make install

# install plotutils

sudo apt -y install plotutils


# install SAT-Reach

cd ..
git clone https://gitlab.com/Atanukundu/SAT-Reach.git
cd ./SAT-Reach/build
sudo make all

# After building SAT-Reach, repeat the results

./SAT-Reach --help
