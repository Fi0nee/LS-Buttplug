from flask import Flask, render_template, request
import serial
import threading
import serial.tools.list_ports
import configparser
import os
import webbrowser
import time
from lovense_module import LovenseLib
from keyboard_module import KeyboardModule

app = Flask(__name__)

CONFIG_FILE = "config.cfg"
ser = None

# ====== Безопасная отправка на ESP32 ======
def safe_send(level):
    global ser
    try:
        if ser and ser.is_open:
            level = max(0, min(20, level))
            ser.write(f"Vibrate:{level};\n".encode())
            time.sleep(0.01)
    except serial.SerialTimeoutException:
        pass
    except Exception:
        pass

lovense = LovenseLib(send_func=safe_send)
keyboard_module = KeyboardModule(send_func=lambda level: safe_send(round(level / 7 * 20)))

config = configparser.ConfigParser()

# ====== Загрузка/создание конфигурации ======
if os.path.exists(CONFIG_FILE):
    config.read(CONFIG_FILE, encoding="utf-8")
else:
    config["Serial"] = {"port": "", "baud_rate": "115200"}
    config["Server"] = {"host": "127.0.0.1", "port": "30010"}
    config["Theme"] = {"color1": "#EE82EE", "color2": "#FF1493"}
    with open(CONFIG_FILE, "w", encoding="utf-8") as f:
        config.write(f)

SERIAL_PORT = config.get("Serial", "port", fallback="")
BAUD_RATE = config.getint("Serial", "baud_rate", fallback=115200)
SERVER_HOST = config.get("Server", "host", fallback="127.0.0.1")
SERVER_PORT = config.getint("Server", "port", fallback=30010)
THEME_COLOR1 = config.get("Theme", "color1", fallback="#EE82EE")
THEME_COLOR2 = config.get("Theme", "color2", fallback="#FF1493")

# ====== Flask Routes ======
@app.route('/')
def index():
    ports = [port.device for port in serial.tools.list_ports.comports()]
    return render_template(
        "index.html",
        com_ports=ports,
        current_com=SERIAL_PORT,
        server_host=SERVER_HOST,
        server_port=SERVER_PORT,
        theme_color1=THEME_COLOR1,
        theme_color2=THEME_COLOR2
    )

@app.route('/setPowerLevel', methods=['POST'])
def set_power_level():
    try:
        level = int(request.form.get('level', '0'))
        level = max(0, min(7, level))
        safe_send(round(level / 7 * 20))
    except ValueError:
        pass
    return '', 204

@app.route('/setComPort', methods=['POST'])
def set_com_port():
    global SERIAL_PORT, ser, config
    port = request.form.get('port')
    SERIAL_PORT = port
    config.set("Serial", "port", SERIAL_PORT)
    with open(CONFIG_FILE, "w", encoding="utf-8") as f:
        config.write(f)
    if ser and ser.is_open:
        ser.close()
    threading.Thread(target=start_serial, daemon=True).start()
    return '', 204

@app.route('/toggleLovenseServer', methods=['POST'])
def toggle_lovense_server():
    state = request.form.get('state')
    if state == 'start':
        threading.Thread(target=lovense.start_server, daemon=True).start()
    else:
        threading.Thread(target=lovense.stop_server, daemon=True).start()
    return '', 204

@app.route('/setServerConfig', methods=['POST'])
def set_server_config():
    global SERVER_HOST, SERVER_PORT, config
    SERVER_HOST = request.form.get('host', SERVER_HOST)
    SERVER_PORT = int(request.form.get('port', SERVER_PORT))
    if not config.has_section("Server"):
        config.add_section("Server")
    config.set("Server", "host", SERVER_HOST)
    config.set("Server", "port", str(SERVER_PORT))
    with open(CONFIG_FILE, "w", encoding="utf-8") as f:
        config.write(f)
    return '', 204

@app.route('/setTheme', methods=['POST'])
def set_theme():
    global THEME_COLOR1, THEME_COLOR2, config
    color1 = request.form.get('color1')
    color2 = request.form.get('color2')
    if not config.has_section("Theme"):
        config.add_section("Theme")
    config.set("Theme", "color1", color1)
    config.set("Theme", "color2", color2)
    THEME_COLOR1 = color1
    THEME_COLOR2 = color2
    with open(CONFIG_FILE, "w", encoding="utf-8") as f:
        config.write(f)
    return '', 204

# ====== Keyboard Mode API ======
@app.route('/toggleKeyboardMode', methods=['POST'])
def toggle_keyboard_mode_route():
    state = request.form.get('state')
    if state == 'on':
        keyboard_module.enable()
    else:
        keyboard_module.disable()
    return '', 204

@app.route('/addKeyBinding', methods=['POST'])
def add_key_binding_route():
    key = request.form.get('key', '').lower()
    try:
        level = int(request.form.get('level', '0'))
        keyboard_module.add_binding(key, level)
    except ValueError:
        pass
    return '', 204

# ====== Serial ======
def start_serial():
    global ser
    try:
        if SERIAL_PORT:
            ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1, write_timeout=2)
            print(f"Connected to {SERIAL_PORT}")
    except Exception as e:
        print(f"Error opening serial port: {e}")

# ====== Run Server ======
if __name__ == '__main__':
    threading.Thread(target=start_serial, daemon=True).start()

    def open_browser():
        time.sleep(1)
        webbrowser.open("http://127.0.0.1:5000")
    threading.Thread(target=open_browser, daemon=True).start()

    app.run(host='0.0.0.0', port=5000)
