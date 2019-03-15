import argparse
import random

if __name__ == '__main__':
    # Parse num secrets from command line
    parser = argparse.ArgumentParser()
    parser.add_argument("num_secrets", type=int)
    args = parser.parse_args()
    N = args.num_secrets

    # Read list of fruits
    infile = open('fruit_list.txt')
    fruits = [f.strip('\n') for f in infile]
    infile.close()

    # Write to new file (overwrite if exists)
    outfile = open('secret_list.txt', 'w')

    for i in range(N):
        s = '%s%d\n' % (random.choice(fruits), int(random.random()*1000))
        outfile.write(s)
    outfile.close()

    


