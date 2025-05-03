import sys
import subprocess
import os
import shutil
import zipfile
from pathlib import Path

def install_requirements():
    # Install packages from requirements.txt
    subprocess.check_call([sys.executable, "-m", "pip", "install", "-r", "requirements.txt"])

def check_and_install_requirements():
    try:
        import yaml
        import schema
        import typing_extensions
        import websockets
    except ImportError:
        print("Some required packages are missing. Installing...")
        install_requirements()

# Check and install requirements if necessary
check_and_install_requirements()


site_packages = os.path.dirname(os.__file__)
output_dir = sys.argv[1]
minipelago_zip = os.path.join(output_dir, "minipelago.zip")
print(f"Packaging MMRecompRando to {minipelago_zip}")

standard_lib_dependencies = [
    "__future__.py",
    "_compat_pickle.py",
    "_weakrefset.py",
    "argparse.py",
    "ast.py",
    "base64.py",
    "bisect.py",
    "collections",
    "concurrent",
    "contextlib.py",
    "contextvars.py",
    "copy.py",
    "copyreg.py",
    "csv.py",
    "dataclasses.py",
    "datetime.py",
    "dis.py",
    "encodings",
    "enum.py",
    "fnmatch.py",
    "functools.py",
    "gettext.py",
    "hashlib.py",
    "heapq.py",
    "hmac.py",
    "importlib",
    "inspect.py",
    "ipaddress.py",
    "json",
    "keyword.py",
    "linecache.py",
    "locale.py",
    "logging",
    "nturl2path.py",
    "numbers.py",
    "opcode.py",
    "operator.py",
    "pathlib.py",
    "pickle.py",
    "platform.py",
    "queue.py",
    "random.py",
    "re",
    "reprlib.py",
    "secrets.py",
    "shlex.py",
    "shutil.py",
    "signal.py",
    "string.py",
    "struct.py",
    "subprocess.py",
    "tempfile.py",
    "textwrap.py",
    "threading.py",
    "token.py",
    "tokenize.py",
    "traceback.py",
    "types.py",
    "typing.py",
    "urllib",
    "warnings.py",
    "weakref.py",
    "websockets",
    "zipfile.py",
    "selectors.py"
]

package_dependencies = [
    "yaml",
    "websockets",
    "schema.py",
    "typing_extensions.py",
]

dummy_dependencies = [
    "email",
    "http",
    "asyncio.py",
    "socket.py",
]

# Function to add a file to the zip archive
def add_file_to_zip(zipf, src, arcname):
    zipf.write(src, arcname)

# Convert a file into a C array
def bin_to_c_array(input_path, output_path, var_name):
    with open(input_path, "rb") as f:
        data = f.read()

    with open(output_path, "w") as out:
        out.write(f"unsigned char {var_name}[] = {{\n")
        for i, byte in enumerate(data):
            if i % 12 == 0:
                out.write("  ")
            out.write(f"0x{byte:02x}, ")
            if (i + 1) % 12 == 0:
                out.write("\n")
        out.write(f"\n}};\nunsigned int {var_name}_len = {len(data)};\n")

# Copy minipelago files
with zipfile.ZipFile(minipelago_zip, 'w', zipfile.ZIP_DEFLATED) as zipf:
    add_file_to_zip(zipf, "archipelago/BaseClasses.py", "BaseClasses.py")
    add_file_to_zip(zipf, "archipelago/Generate.py", "Generate.py")
    add_file_to_zip(zipf, "archipelago/Fill.py", "Fill.py")
    add_file_to_zip(zipf, "archipelago/Main.py", "Main.py")
    add_file_to_zip(zipf, "minipelago/scripts/DummyModuleUpdate.py", "ModuleUpdate.py")
    add_file_to_zip(zipf, "archipelago/MultiServer.py", "MultiServer.py")
    add_file_to_zip(zipf, "archipelago/NetUtils.py", "NetUtils.py")
    add_file_to_zip(zipf, "archipelago/Options.py", "Options.py")
    add_file_to_zip(zipf, "archipelago/Utils.py", "Utils.py")
    add_file_to_zip(zipf, "archipelago/settings.py", "settings.py")
    add_file_to_zip(zipf, "archipelago/requirements.txt", "requirements.txt")
    add_file_to_zip(zipf, "minipelago/scripts/MMGenerate.py", "MMGenerate.py")

    # Copy worlds files
    add_file_to_zip(zipf, "minipelago/scripts/worlds_init.py", "worlds/__init__.py")
    add_file_to_zip(zipf, "archipelago/worlds/AutoSNIClient.py", "worlds/AutoSNIClient.py")
    add_file_to_zip(zipf, "archipelago/worlds/AutoWorld.py", "worlds/AutoWorld.py")
    add_file_to_zip(zipf, "archipelago/worlds/Files.py", "worlds/Files.py")
    add_file_to_zip(zipf, "archipelago/worlds/LauncherComponents.py", "worlds/LauncherComponents.py")
    add_file_to_zip(zipf, "archipelago/worlds/alttp/EntranceRandomizer.py", "worlds/alttp/EntranceRandomizer.py")
    add_file_to_zip(zipf, "archipelago/worlds/alttp/Text.py", "worlds/alttp/Text.py")
    add_file_to_zip(zipf, "minipelago/scripts/__init__.py", "worlds/alttp/__init__.py")

    # Copy necessary worlds
    for world in ["generic", "mm_recomp"]:
        world_root = os.path.join("archipelago", "worlds", world)
        for root, dirs, files in os.walk(world_root):
            for file in files:
                full_path = os.path.join(root, file)
                rel_path = os.path.relpath(full_path, world_root)
                if rel_path.startswith("docs/") or rel_path.startswith("test/"):
                    continue

                dest_path = os.path.join("worlds", world, rel_path)
                add_file_to_zip(zipf, full_path, dest_path)

    # Copy necessary dependencies
    for package in standard_lib_dependencies:
        package_root = os.path.join(site_packages, package)
        if package.endswith(".py"):
            add_file_to_zip(zipf, package_root, package)
        else:
            for root, dirs, files in os.walk(package_root):
                for file in files:
                    if not file.endswith(".pyc"):
                        full_path = os.path.join(root, file)
                        rel_path = os.path.relpath(full_path, package_root)
                        dest_path = os.path.join(package, rel_path)
                        add_file_to_zip(zipf, full_path, dest_path)

    # Copy necessary dependencies
    for package in package_dependencies:
        package_root = os.path.join(site_packages, "site-packages", package)
        if package.endswith(".py"):
            add_file_to_zip(zipf, package_root, package)
        else:
            for root, dirs, files in os.walk(package_root):
                for file in files:
                    if not file.endswith(".pyc"):
                        full_path = os.path.join(root, file)
                        rel_path = os.path.relpath(full_path, package_root)
                        dest_path = os.path.join(package, rel_path)
                        add_file_to_zip(zipf, full_path, dest_path)

    # Copy dummy dependencies
    for package in dummy_dependencies:
        package_root = os.path.join("minipelago/dummy_modules", package)
        if package.endswith(".py"):
            add_file_to_zip(zipf, package_root, package)
        else:
            for root, dirs, files in os.walk(package_root):
                for file in files:
                    if not file.endswith(".pyc"):
                        full_path = os.path.join(root, file)
                        rel_path = os.path.relpath(full_path, package_root)
                        dest_path = os.path.join(package, rel_path)
                        add_file_to_zip(zipf, full_path, dest_path)

minipelago_zip_c = minipelago_zip + ".c"

bin_to_c_array(minipelago_zip, minipelago_zip_c, "minipelago_zip")

print(f"Packaging and array conversion complete in {minipelago_zip_c}")
