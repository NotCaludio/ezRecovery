import os
import win32file


def read_physical_volume(volume_number):
    # Open the physical volume using the \\.\PhysicalDrive syntax
    volume_path = f"\\\\.\\PhysicalDrive{volume_number}"
    
    try:
        # Open the volume in binary mode for reading
        volume_handle = win32file.CreateFile(
            volume_path,
            win32file.GENERIC_READ,
            win32file.FILE_SHARE_READ | win32file.FILE_SHARE_WRITE,
            None,
            win32file.OPEN_EXISTING,
            0,
            None
        )
        
        # Get the size of the physical volume
        volume_size = win32file.GetFileSize(volume_handle)
        
        # Read the bytes from the physical volume
        volume_bytes = win32file.ReadFile(volume_handle, volume_size)[1]

        win32file.CloseHandle(volume_handle)
        
        return volume_bytes
    except Exception as e:
        print(f"An error occurred while reading physical volume {volume_number}: {str(e)}")
        

    return None

print(read_physical_volume("E"))