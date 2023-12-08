import subprocess
import matplotlib.pyplot as plt


N_TESTS_AVG = 2


def run_serial(num_words):
    # Run the serial binary
    result = 0
    for _ in range(N_TESTS_AVG):
        result += float(subprocess.run(['./serial', str(num_words)], capture_output=True, text=True, check=True).stdout)
    return result / N_TESTS_AVG


def run_omp(num_threads, num_words):
    # Run the omp_parallel binary with specified number of threads as processes
    result = 0
    for _ in range(N_TESTS_AVG):
        result += float(subprocess.run(['./omp_parallel', str(num_threads), str(num_words)], capture_output=True, text=True, check=True).stdout)
    return result / N_TESTS_AVG


def run_mpi(num_processes, num_words):
    # Run the mpi_parallel binary with specified number of processes
    result = 0
    for _ in range(N_TESTS_AVG):
        result += float(subprocess.run(['mpirun', '-n', str(num_processes), './mpi_parallel', str(num_words)], capture_output=True, text=True, check=True).stdout)
    return result / N_TESTS_AVG


def thread_test(thread_tests, num_words):

    serial_result = 0.0
    omp_results = []
    mpi_results = []

    print(f"Running Thread tests with {num_words} words...")

    print(f"Running serial with {num_words} words...")
    serial_result = run_serial(num_words)
    for num_threads in thread_tests:
        print(f"Running omp with {num_threads} threads and {num_words} words...")
        omp_results.append(run_omp(num_threads, num_words))
        print(f"Running mpi with {num_threads} processes and {num_words} words...")
        mpi_results.append(run_mpi(num_threads, num_words))

    return [serial_result for _ in range(len(thread_tests))], omp_results, mpi_results


def word_test(word_tests, num_threads):

    serial_results = []
    omp_results = []
    mpi_results = []

    print(f"Running Word tests with {num_threads} threads...")

    for num_words in word_tests:
        print(f"Running serial with {num_words} words...")
        serial_results.append(run_serial(num_words))
        print(f"Running omp with {num_threads} threads and {num_words} words...")
        omp_results.append(run_omp(num_threads, num_words))
        print(f"Running mpi with {num_threads} processes and {num_words} words...")
        mpi_results.append(run_mpi(num_threads, num_words))

    return serial_results, omp_results, mpi_results


if __name__ == "__main__":
    thread_tests = [1, 2, 4, 8, 16, 32]
    word_tests = [50, 100, 500, 1000, 1500, 2000]

    # run thread test and plot graph
    serial_results, omp_results, mpi_results = thread_test(thread_tests, 100)
    plt.plot(thread_tests, serial_results, label='Serial')
    plt.plot(thread_tests, omp_results, label='OpenMP')
    plt.plot(thread_tests, mpi_results, label='MPI')
    plt.xlabel('Number of Threads')
    plt.ylabel('Time (s)')
    plt.legend()
    plt.savefig('thread_graph.png')

    # clear fig
    plt.clf()

    # run word test and plot graph
    serial_results, omp_results, mpi_results = word_test(word_tests, 4)
    plt.plot(word_tests, serial_results, label='Serial')
    plt.plot(word_tests, omp_results, label='OpenMP')
    plt.plot(word_tests, mpi_results, label='MPI')
    plt.xlabel('Number of Words')
    plt.ylabel('Time (s)')
    plt.legend()
    plt.savefig('word_graph.png')