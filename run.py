##run this file to simulate for different capacity
import os
capList = [1,2,3,4,5,6,7,8,9,10]
for cap in capList:
    print ('Running for capacity: ' + str(cap) + 'Mbps')
    os.system('cd ns-3.29' + ' && ' + './waf --run "cs621-dev01 --cap=' + str(cap) + '"')
