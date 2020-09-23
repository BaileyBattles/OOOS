import base64
import math
import os

FAT_ENTRY_SIZE = 2 #bytes
SECTORS_PER_CLUSTER = 6
SECTOR_SIZE = 512
FAT_START_CLUSTER = 1 #second cluster
TOTAL_CLUSTERS = 65536
CLUSTER_SIZE = SECTORS_PER_CLUSTER * SECTOR_SIZE
DISK_NAME = os.path.join(os.getcwd(), 'drive/storage.img')

ROOT_CLUSTER = FAT_START_CLUSTER + int(TOTAL_CLUSTERS * FAT_ENTRY_SIZE / CLUSTER_SIZE)

def write_root_and_bin_cluster(filename):
    ##Important: ByteString requires the above sizes to hold true.
    ##the \x2b below refers to the cluster number 0x2b->32
    ##For this to work RootCluster = 43
    if ROOT_CLUSTER != 43:
        print('Root cluster must be 43!')
        return
    
    byteString = \
    b'\x2e\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00\x00' + \
    b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x2b\x00\x00\x00\x00\x00' + \
    b'\x2e\x2e\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00\x00' + \
    b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x2b\x00\x00\x00\x00\x00' + \
    b'\x42\x49\x4E\x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00\x00' + \
    b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x2c\x00\x00\x00\x00\x00'
    with open(filename, 'r+b') as f:
        f.seek((ROOT_CLUSTER) * SECTORS_PER_CLUSTER * SECTOR_SIZE)
        f.write(byteString)
    set_fat_entry(filename, b'\xF1\xFF', ROOT_CLUSTER)

    bin_cluster = ROOT_CLUSTER + 1
    byteString = \
    b'\x2e\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00\x00' + \
    b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x2b\x00\x00\x00\x00\x00' + \
    b'\x2e\x2e\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00\x00' + \
    b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x2c\x00\x00\x00\x00\x00'
    with open(filename, 'r+b') as f:
        f.seek((bin_cluster) * SECTORS_PER_CLUSTER * SECTOR_SIZE)
        f.write(byteString)
    set_fat_entry(filename, b'\xF1\xFF', bin_cluster)

#index should be a byte string
def set_fat_entry(filename, entry, index):
    with open(filename, 'r+b') as f:
        f.seek(FAT_START_CLUSTER * SECTORS_PER_CLUSTER * SECTOR_SIZE + FAT_ENTRY_SIZE * index)
        f.write(entry)

def write_fat(disk_file_name):
    fat_entries_per_sector = SECTOR_SIZE / FAT_ENTRY_SIZE
    num_fat_clusters = TOTAL_CLUSTERS / (fat_entries_per_sector * SECTORS_PER_CLUSTER)
    num_fat_clusters = math.ceil(num_fat_clusters)
    with open(disk_file_name, 'r+b') as f:
        f.seek(FAT_START_CLUSTER * SECTORS_PER_CLUSTER * SECTOR_SIZE)
        reserved = b'\xF0\xFF'
        for i in range(num_fat_clusters):
            f.write(reserved)

def write_fileentry_to_bin(disk_file_name, filename, start_cluster, filenumber, filesize):
    with open(disk_file_name, 'r+b') as disk_file:
        b = bytearray()
        b.extend(map(ord, filename))
        filename_bytes = bytes(b)
        filename_filler = bytes(8 - len(filename))
        start_cluster_bytes = start_cluster.to_bytes(2, byteorder='little')
        size_bytes = filesize.to_bytes(4, byteorder='little')
        file_entry = filename_bytes + \
        filename_filler + \
        b'\x00\x00\x00\x00\x00\x00\x00\x00' + \
        b'\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00' + \
        start_cluster_bytes + \
        size_bytes
        
        bin_cluster = ROOT_CLUSTER + 1
        disk_file.seek(bin_cluster * CLUSTER_SIZE + 32 * filenumber)
        disk_file.write(file_entry)
        
def write_file_to_bin(filename, disk_file_name, filenumber, currClusterNum):
    start_cluster = currClusterNum
    contents = b''
    with open(filename[0], 'rb') as data_file:
        with open(disk_file_name, 'r+b') as disk_file:
            contents = data_file.read()
            num_clusters = int(len(contents) / (CLUSTER_SIZE)) + 1
            print(num_clusters)
            for i in range(num_clusters - 1):
                nextCluster = currClusterNum + 1
                byte_entry = nextCluster.to_bytes(2, byteorder='little')
                set_fat_entry(disk_file_name, byte_entry, currClusterNum)
                currClusterNum += 1
            
            set_fat_entry(disk_file_name, b'\xF8\xFF', currClusterNum)
            disk_file.seek(start_cluster * CLUSTER_SIZE)
            disk_file.write(contents)
            write_fileentry_to_bin(disk_file_name, filename[1], start_cluster, filenumber, len(contents))
    return (filenumber + 1, currClusterNum + 1)


def main():
    #THIS IS BAD CODE!!!!!!
    #STOP BEING LAZY AND FIX IT
    #DONT CHANGE FIRST FEW LINES
    #TO WRITE NEW FILE ADD ANOTHER (filenumber, currClusterNum) =write_file_to_bin(filename, DISK_NAME, filenumber, currClusterNum)
    write_fat(DISK_NAME)
    filenames = []
    filenames.append((os.path.join(os.getcwd(), 'src/Userland/sh.o'), 'SH'))
    filenames.append((os.path.join(os.getcwd(), 'src/Userland/ls.o'), 'LS'))

    write_root_and_bin_cluster(DISK_NAME)
    filenumber = 2 #Pass over dot and double dot
    currClusterNum = ROOT_CLUSTER + 2 #We start with the cluster after BIN
    for filename in filenames:
        (filenumber, currClusterNum) = write_file_to_bin(filename, DISK_NAME, filenumber, currClusterNum)
    #(filenumber, currClusterNum) = write_file_to_bin(filename, DISK_NAME, filenumber, currClusterNum)

if __name__ == '__main__':
    main()

