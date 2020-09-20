from sys import stdin

# Parse latencies
latencies = []	# List of stored latencies
sample_count = 0	# Count of samples collected
packet_loss = ""
for line in stdin:
	if "time=" in line:	# Parse and add to set
		sample_count += 1
		print("Got", sample_count, "samples", " "*20, end = "\r")
		latency = line.split("time=")[1]
		latency = latency.split(" ")[0]
		latency = float(latency)
		latencies.append(latency)
	elif "packet loss" in line:
		loss_data = line.split(",")
		packet_loss = loss_data[2].strip()

print()
latencies.sort()

# Print 50th, 90th and 99th percentile (greater than) (Assuming 100 packets)
print("50th percentile:", latencies[50])
print("90th percentile:", latencies[90])
print("99th percentile:", latencies[99])
print("Average:", sum(latencies)/len(latencies))
print(packet_loss)