# Compression Detection Application Technical Manual

## Installation/Usage Guide
# Tools required
* [ns3] (https://www.nsnam.org/) - Network Simulator
* [Python] (https://www.python.org/) - Programming Language
* [zlib] (https://www.zlib.net/) - Compression Library
  
# Steps
## 1. NS-3.29
1) ***Install ns-3.29***  
Type the following on the Linux shell:  
  
```cd```  
```mkdir workspace```  
```cd workspace```  
```wget https://www.nsnam.org/release/ns-allinone-3.29.tar.bz2```  
```tar xjf ns-allinone-3.29.tar.bz2```  
  
2) ***Build***  
If you downloaded using a tarball you should have a directory called something like ns-allinone-3.29 under your ~/workspace directory. Type the following:  
  
```./build.py --enable-examples --enable-tests```  
  
3) ***Test for installation***  
You can run the unit tests of the ns-3 distribution by running the ./test.py script:  
  
```./test.py```

## 2. zlib
1) ***Install zlib***  
  
```wget http://zlib.net/zlib-1.2.11.tar.gz```  
```tar zxf zlib-1.2.11.tar.gz```  
```cd zlib-1.2.11```  
```./configure```  
```make```  
```make check```  
```sudo make install```  

## 3. Application 
1) ***Run***  
run.py using commandline: "python run.py"  
  
  

