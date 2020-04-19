import os

FAT_ENTRY_SIZE = 2 #bytes
SECTORS_PER_CLUSTER = 6
SECTOR_SIZE = 512
FAT_START_CLUSTER = 1 #second cluster
TOTAL_CLUSTERS = 65536
CLUSTER_SIZE = SECTORS_PER_CLUSTER * SECTOR_SIZE
DISK_NAME = os.path.join(os.getcwd(), 'drive/storage.img')

ROOT_CLUSTER = int(FAT_START_CLUSTER + TOTAL_CLUSTERS * FAT_ENTRY_SIZE / CLUSTER_SIZE)

def write_file(filename):
    
    with open(DISK_NAME) as disk:
        with open(filename) as f:
            content = f.read()
            print(content)
    print(ROOT_CLUSTER)

def main():
    filename = os.path.join(os.getcwd(), 'ex.txt')
    write_file(filename)

if __name__ == '__main__':
    main()

