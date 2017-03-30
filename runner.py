import subprocess
import matplotlib.pyplot as plt

# times = [];
# for i in range(5,23):
# 	filename = 'file'+str(i)
# 	print filename, i
# 	print subprocess.check_output(['bash', 'run.sh', filename]);

# times = [0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.004, 0.008, 0.016, 0.032, 0.048, 0.116, 0.224, 0.452, 0.924, 1.892, 3.732, 7.728]
times = [0.001, 0.001, 0.001, 0.001, 0.001, 0.001, 0.004, 0.008, 0.016, 0.032, 0.048, 0.116, 0.224, 0.452, 0.924, 1.892]

plt.plot([2**(i-3) for i in range(5,21)], times)
# plt.xticks([2**i for i in range(5,21)], ['2^'+str(i) for i in range(5,21)])
plt.xlabel('File Size (in KB)')
plt.ylabel('CPU Time in (s)')
plt.title('Performance')
plt.show()