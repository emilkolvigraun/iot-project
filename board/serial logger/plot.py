import matplotlib.pyplot as plt
import pandas

def cumulate(data):
    cumulative = []
    prev_value = 0
    for i in data.index:
        prev_value += data.amphere[i]
        cumulative.append(prev_value)
    return range(len(cumulative)), cumulative 

plain = pandas.read_csv('plain.txt').dropna().loc[0:216000]
bwifi = pandas.read_csv('bwifi.txt').dropna().loc[0:216000]

p_x, p_y = cumulate(plain)
w_x, w_y = cumulate(bwifi)

print('wifi: ', sum(bwifi.amphere))
print('plain: ', sum(plain.amphere))

plt.xlabel("Time [milliseconds]") 
plt.ylabel("Cummulative Current [Amphere]")

plt.plot(p_x, p_y, label="plain", lw=3)
plt.plot(w_x, w_y, label="wifi", lw=3)
plt.legend()
plt.show() 