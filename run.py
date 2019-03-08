##run this file to simulate for different capacity
import os
capList = [1,2,3,4,5,6,7,8,9,10]
compression = [0,1]
for cap in capList:
    for comp in compression:
        if comp == 0:
            print ('Running for capacity: ' + str(cap) + 'Mbps' + ' without compression')
            os.system('cd ns-3.29' + ' && ' + './waf --run "cs621-dev01 --cap=' + str(cap) + ' --comp=0"')
        else:
            print ('Running for capacity: ' + str(cap) + 'Mbps' + ' with compression')
            os.system('cd ns-3.29' + ' && ' + './waf --run "cs621-dev01 --cap=' + str(cap) + ' --comp=1"')
