import win32api
import win32con

def get_folder_attributes(folder_path):
    try:
        # Get the folder attributes using win32api
        attributes = win32api.GetFileAttributes(folder_path)
        
        # Check specific attributes
        is_readonly = attributes & win32con.FILE_ATTRIBUTE_READONLY
        is_hidden = attributes & win32con.FILE_ATTRIBUTE_HIDDEN
        is_system = attributes & win32con.FILE_ATTRIBUTE_SYSTEM
        is_directory = attributes & win32con.FILE_ATTRIBUTE_DIRECTORY
        is_archive = attributes & win32con.FILE_ATTRIBUTE_ARCHIVE
        is_normal = attributes & win32con.FILE_ATTRIBUTE_NORMAL
        
        # Print the folder attributes
        print(f"Folder Path: {folder_path}")
        print(f"Read-only: {'Yes' if is_readonly else 'No'}")
        print(f"Hidden: {'Yes' if is_hidden else 'No'}")
        print(f"System: {'Yes' if is_system else 'No'}")
        print(f"Directory: {'Yes' if is_directory else 'No'}")
        print(f"Archive: {'Yes' if is_archive else 'No'}")
        print(f"Normal: {'Yes' if is_normal else 'No'}")
    
    except Exception as e:
        print(f"An error occurred: {str(e)}")

# Example usage
folder_path = r"C:\Users\claud\Documents\Sexto_semestre\proyecto_integracion_tecnologica\ezRecovery\test"
get_folder_attributes(folder_path)