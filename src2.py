import pyfsntfs
import os

def recover_files(partition_path, output_directory):
    # Open the NTFS partition
    file_system = pyfsntfs.file_system(partition_path)

    # Iterate over all MFT entries
    for mft_entry in file_system.mft_entries():
        # Check if the MFT entry is allocated (not deleted)
        if mft_entry.is_in_use():
            continue

        # Get the file name
        file_name = mft_entry.file_name_attribute().name

        # Create the output file path
        output_path = os.path.join(output_directory, file_name)

        # Check if the file already exists
        if os.path.exists(output_path):
            continue

        # Open the MFT entry as a file object
        with mft_entry.open_data_stream() as source_file:
            # Open the output file for writing
            with open(output_path, 'wb') as destination_file:
                # Copy the data from the source file to the destination file
                destination_file.write(source_file.read())

        print(f"Recovered file: {file_name}")
        break

## Specify the path to the NTFS partition
#partition_path = r'C:'
#
## Specify the output directory for recovered files
#output_directory = r'C:\Users\claud\Documents\Sexto_semestre\proyecto_integracion_tecnologica\ezRecovery\test\recovered'
#
## Create the output directory if it doesn't exist
#os.makedirs(output_directory, exist_ok=True)
#
## Call the function to recover files
#recover_files(partition_path, output_directory)

import pytsk3
import os

def recover_files(partition_path, output_directory):
    # Open the NTFS partition
    img = pytsk3.Img_Info(partition_path)
    fs = pytsk3.FS_Info(img)

    # Iterate over all files in the file system
    for root_dir, dirs, files in fs.walk():
        print(r"C:\Users\claud\Documents\Sexto_semestre\proyecto_integracion_tecnologica\ezRecovery\test")
        with open(r"C:\Users\claud\Documents\Sexto_semestre\proyecto_integracion_tecnologica\ezRecovery\test\holassss.txt", 'w') as file:
            file.write("holaalallala")
        break
        for file in files:
            # Check if the file is deleted
            if file.info.meta.flags & pytsk3.TSK_FS_META_FLAG_UNALLOC:
                # Get the file name
                file_name = file.info.name.name.decode('utf-8')

                # Create the output file path
                output_path = os.path.join(output_directory, file_name)

                # Check if the file already exists
                if os.path.exists(output_path):
                    continue

                # Open the file object
                file_obj = fs.open_meta(file.info.meta.addr)

                # Open the output file for writing
                with open(output_path, 'wb') as destination_file:
                    offset = 0
                    size = file.info.meta.size
                    while offset < size:
                        available_to_read = min(4096, size - offset)
                        data = file_obj.read_random(offset, available_to_read)
                        if not data:
                            break
                        destination_file.write(data)
                        offset += len(data)

                print(f"Recovered file: {file_name}")
                break

# Specify the path to the NTFS partition
partition_path = r'\\.\C:'

# Specify the output directory for recovered files
output_directory = r'C:\Users\claud\Documents\Sexto_semestre\proyecto_integracion_tecnologica\ezRecovery\test\recovered'

# Create the output directory if it doesn't exist
os.makedirs(output_directory, exist_ok=True)

# Call the function to recover files
recover_files(partition_path, output_directory)