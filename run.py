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


result = [100, 300, 200, 500, 400, 100, 300, 200, 500, 400]

import numpy as np
import matplotlib.pyplot as plt
 
# Make a graph. 
def makeGraph(capacity, time_diff): 
    plt.scatter(capacity, time_diff)
    #plt.plot(capacity, time_diff)
    plt.title("Simulation Result")
    plt.xlabel("capacity")
    plt.ylabel("Dt_H - Dt_L")
    plt.show()

capacity = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10])
time_diff = np.array(result)
makeGraph(capacity, time_diff)
