import subprocess
import matplotlib.pyplot as plt

# List of binary names
binary_names = ['serial', 'omp_parallel', 'mpi_parallel']

# Dictionary to store total run times for each binary and thread count
total_runtimes = {binary_name: {threads: 0.0 for threads in [1, 4, 8]} for binary_name in binary_names}

# Number of times to run each binary with different thread counts
num_runs_omp_mpi = 10

# Number of times to run serial
num_runs_serial = 1

for binary_name in binary_names:
    for num_threads in [1, 4, 8]:
        total_runtime = 0.0

        # Determine the number of runs based on the binary
        num_runs = num_runs_omp_mpi if binary_name in ['omp_parallel', 'mpi_parallel'] else num_runs_serial

        for _ in range(num_runs):
            try:
                if binary_name == 'mpi_parallel':
                    # Run the mpi_parallel binary with mpirun and specified number of threads as processes
                    result = subprocess.run(['mpirun', '-np', str(num_threads), './' + binary_name, str(num_threads)], capture_output=True, text=True, check=True)
                    print(f"mpi_parallel run count {_} at {num_threads}")
                elif binary_name == 'omp_parallel':
                    # Run the omp_parallel binary with the specified number of threads
                    result = subprocess.run(['./' + binary_name, str(num_threads)], capture_output=True, text=True, check=True)
                    print(f"omp_parallel run count {_} at {num_threads}")
                else:
                    # Run other binaries without arguments
                    result = subprocess.run(['./' + binary_name], capture_output=True, text=True, check=True)
                    print(f"serial run count {_} at {num_threads}")

                # Extract and accumulate the total run time
                output_lines = result.stdout.strip().split('\n')
                if len(output_lines) == 1:
                    total_runtime += float(output_lines[0])
                else:
                    print("Unexpected output format from {}: {}".format(binary_name, result.stdout))

            except subprocess.CalledProcessError as e:
                # If the subprocess returns a non-zero exit code, print an error message
                print("Error running {}: {}".format(binary_name, e.stderr))

        # Update the total run time for the current binary and thread count
        total_runtimes[binary_name][num_threads] = total_runtime
        print("{} ({} Threads/Processes) - Total Run Time: {:.4f} seconds".format(binary_name, num_threads, total_runtime))

# Print the total run times dictionary
print("\nTotal Runtimes:")
for binary_name, thread_runtimes in total_runtimes.items():
    for num_threads, total_runtime in thread_runtimes.items():
        print("{} ({} Threads/Processes): {:.4f} seconds".format(binary_name, num_threads, total_runtime))

# Create a bar graph for omp_parallel and mpi_parallel with different thread counts
for binary_name in ['omp_parallel', 'mpi_parallel']:
    plt.bar([f"{binary_name} ({num_threads} Threads/Processes)" for num_threads in [1, 4, 8]], [total_runtimes[binary_name][num_threads] for num_threads in [1, 4, 8]])
    plt.ylabel('Total Run Time (seconds)')
    plt.title(f'Total Run Time for {binary_name} with Different Thread Counts/Processes')
    plt.savefig(f'{binary_name}_thread_process_runtimes.png')
    plt.show()
