import base64
import os

FAT_ENTRY_SIZE = 2 #bytes
SECTORS_PER_CLUSTER = 6
SECTOR_SIZE = 512
FAT_START_CLUSTER = 1 #second cluster
TOTAL_CLUSTERS = 65536
CLUSTER_SIZE = SECTORS_PER_CLUSTER * SECTOR_SIZE
DISK_NAME = os.path.join(os.getcwd(), 'drive/storage.img')

ROOT_CLUSTER = FAT_START_CLUSTER + int(TOTAL_CLUSTERS * FAT_ENTRY_SIZE / CLUSTER_SIZE)

def write_root_cluster(filename):
    byteString = \
    b'\x2e\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00\x00' + \
    b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x2b\x00\x00\x00\x00\x00' + \
    b'\x2e\x2e\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00\x00' + \
    b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x2b\x00\x00\x00\x00\x00' + \
    b'\x4C\x49\x42\x00\x00\x00\x00\x00\x00\x00\x00\x56\x00\x00\x00\x00' + \
    b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x2c\x00\x00\x00\x00\x00'
    with open(filename, 'r+b') as f:
        f.seek((ROOT_CLUSTER) * SECTORS_PER_CLUSTER * SECTOR_SIZE)
        f.write(byteString)


def write_file(filename):
    print(ROOT_CLUSTER)

def main():
    filename = os.path.join(os.getcwd(), 'ex.txt')
    write_root_cluster(DISK_NAME)
    write_file(filename)

if __name__ == '__main__':
    main()

