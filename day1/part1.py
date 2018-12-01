import sys


def main(file):
    with open(file, 'r') as raw_input:
        result = 0
        for line in raw_input:
            result += int(line)
        print(result)


if __name__ == '__main__':
    file = sys.argv[1]
    main(file)
