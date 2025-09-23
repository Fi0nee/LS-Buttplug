import subprocess
import os
import glob
import re
import sys
import serial.tools.list_ports
from concurrent.futures import ThreadPoolExecutor


class ESP32Flasher:
    def __init__(self, esp_dir=None):
        self.esp_dir = esp_dir or os.path.join(os.getcwd(), "ESP")
        if not os.path.isdir(self.esp_dir):
            raise FileNotFoundError(f"ESP folder not found: {self.esp_dir}")

        # chip → firmware folder mapping
        self.chip_map = {
            "esp32": "ESP32",
            "esp32c3": "ESP32c3",
            "esp32-c3": "ESP32c3",
        }

        # standard binary addresses
        self.addr_map = {
            "bootloader": "0x1000",
            "partitions": "0x8000",
            "firmware": "0x10000",
        }

    def detect_chip(self, port, baudrate=115200):
        """Detect chip type using esptool"""
        cmd = ["esptool", "--port", port, "--baud", str(baudrate), "chip_id"]
        result = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
        output = result.stdout.lower()

        if "esp32-c3" in output:
            return "esp32c3"
        elif "esp32" in output:
            return "esp32"
        return None  # unknown chip

    def get_firmware_files(self, chip_name):
        """Return list of binaries to flash"""
        folder = self.chip_map.get(chip_name, chip_name)
        fw_dir = os.path.join(self.esp_dir, folder)

        if not os.path.isdir(fw_dir):
            raise FileNotFoundError(f"Firmware folder not found for {chip_name}: {fw_dir}")

        binaries = []
        for name, addr in self.addr_map.items():
            pattern = os.path.join(fw_dir, f"{name}*.bin")
            files = glob.glob(pattern)
            if not files:
                raise FileNotFoundError(f"File {name}*.bin not found in {fw_dir}")
            binaries.append((addr, files[0]))

        return binaries

    def flash(self, port, baudrate=115200):
        """Start flashing process"""
        chip = self.detect_chip(port, baudrate)
        if not chip:
            raise RuntimeError(f"Failed to detect chip on port {port}")

        print(f"\n✅ Detected chip: {chip} on port {port}")

        binaries = self.get_firmware_files(chip)

        cmd = ["esptool", "--chip", chip, "--port", port, "--baud", str(baudrate), "write_flash", "-z"]
        for addr, path in binaries:
            cmd.extend([addr, path])

        print(f"\n🚀 Starting flash: {' '.join(cmd)}\n")

        process = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, bufsize=1)
        for line in process.stdout:
            line = line.strip()
            if "writing at" in line and "%" in line.lower():
                match = re.search(r'(\d+)\s?%', line)
                if match:
                    print(f"Progress: {match.group(1)} %")
            print(line)
        process.wait()

        if process.returncode == 0:
            print("\n🎉 Flashing completed successfully!")
            return True
        else:
            print("\n❌ Flashing failed!")
            return False


def try_port(p):
    try:
        flasher = ESP32Flasher()
        chip = flasher.detect_chip(p.device)
        return p.device, chip
    except Exception:
        return p.device, None


def auto_choose_port():
    ports = list(serial.tools.list_ports.comports())
    if not ports:
        print("❌ No COM ports found!")
        sys.exit(1)

    print(f"🔍 Checking {len(ports)} ports...")

    with ThreadPoolExecutor(max_workers=len(ports)) as executor:
        results = list(executor.map(try_port, ports))

    for port, chip in results:
        if chip:
            print(f"✅ Detected {chip} on {port}")
            return port
        else:
            print(f"❌ No ESP32 detected on {port}")

    print("❌ Could not detect any ESP chip.")
    sys.exit(1)


if __name__ == "__main__":
    port = auto_choose_port()
    flasher = ESP32Flasher()
    flasher.flash(port)

    input("\nPress Enter to exit...")
