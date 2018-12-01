import sys


def main(file):
    with open(file, 'r') as raw_input:
        lines = raw_input.readlines()

    result_cache = set()
    loop_count = 0
    result = 0
    result_cache.add(0)
    while True:
        loop_count += 1
        print('Iteration: {}'.format(loop_count))

        for line in lines:
            result += int(line)
            if result in result_cache:
                print(result)
                return
            else:
                result_cache.add(result)


if __name__ == '__main__':
    file = sys.argv[1]
    main(file)
