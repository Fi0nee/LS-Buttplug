import threading
import time
import json
import re
import configparser
import os
import socket

MAX_LEVEL_SPOUSE = 7
MAX_LEVEL_LOVENSE = 14

class LovenseLib:
    def __init__(self, send_func, config_file="config.cfg"):
        self.send_func = send_func
        self.pattern_thread = None
        self.pattern_stop_event = threading.Event()
        self.server_thread = None
        self.server_stop_event = threading.Event()
        self.config_file = config_file
        self.HOST = "127.0.0.1"
        self.PORT = 30010
        self._read_config()

    def _read_config(self):
        if os.path.exists(self.config_file):
            cfg = configparser.ConfigParser()
            cfg.read(self.config_file)
            if "Server" in cfg:
                self.HOST = cfg.get("Server", "host", fallback=self.HOST)
                self.PORT = cfg.getint("Server", "port", fallback=self.PORT)

    # ==================== Уровень ====================
    def convert_level(self, lovense_level):
        spouse_level = round((lovense_level / MAX_LEVEL_LOVENSE) * MAX_LEVEL_SPOUSE)
        return max(0, min(MAX_LEVEL_SPOUSE, spouse_level))

    def send_to_spouse(self, level):
        try:
            self.send_func(level)
        except Exception:
            pass

    # ==================== Паттерны ====================
    def parse_rule_delays(self, rule):
        delays = re.findall(r"S:(\d+)", rule)
        return [int(d) / 1000.0 for d in delays] if delays else [0.5]

    def pattern_worker(self, strengths, delays, duration):
        i = 0
        start_time = time.time()
        while not self.pattern_stop_event.is_set():
            self.send_to_spouse(self.convert_level(strengths[i]))
            time.sleep(delays[i] if i < len(delays) else delays[-1])
            i = (i + 1) % len(strengths)
            if duration > 0 and time.time() - start_time >= duration:
                break
        self.send_to_spouse(0)

    def start_pattern(self, strength_str, rule, time_sec):
        try:
            strengths = [int(s) for s in strength_str.split(";")]
        except ValueError:
            return
        delays = self.parse_rule_delays(rule)
        if len(delays) == 1 and len(strengths) > 1:
            delays *= len(strengths)

        self.stop_pattern()
        self.pattern_stop_event.clear()
        duration = float(time_sec) if time_sec else 0
        self.pattern_thread = threading.Thread(
            target=self.pattern_worker,
            args=(strengths, delays, duration),
            daemon=True
        )
        self.pattern_thread.start()

    def stop_pattern(self):
        self.pattern_stop_event.set()
        if self.pattern_thread and self.pattern_thread.is_alive():
            self.pattern_thread.join(timeout=1)
        self.pattern_thread = None

    # ==================== Команды ====================
    def handle_function_action(self, action_str, time_sec):
        parts = action_str.split(",")
        levels = []
        for p in parts:
            try:
                _, val = p.split(":")
                levels.append(int(val))
            except ValueError:
                continue
        if levels:
            max_level = max(levels)
            spouse_lvl = self.convert_level(max_level)
            self.send_to_spouse(spouse_lvl)
            if time_sec and float(time_sec) > 0:
                threading.Timer(float(time_sec), lambda: self.send_to_spouse(0)).start()

    def handle_command(self, cmd_json):
        try:
            cmd = json.loads(cmd_json)
        except json.JSONDecodeError:
            return

        command = cmd.get("command", "")
        time_sec = cmd.get("timeSec", 0)

        if command == "Function":
            action = cmd.get("action", "")
            if action == "Stop":
                self.stop_pattern()
                self.send_to_spouse(0)
            elif action.startswith("Vibrate:") and "," not in action:
                self.stop_pattern()
                try:
                    level = int(action.split(":")[1])
                    self.send_to_spouse(self.convert_level(level))
                    if time_sec and float(time_sec) > 0:
                        threading.Timer(float(time_sec), lambda: self.send_to_spouse(0)).start()
                except ValueError:
                    pass
            else:
                self.stop_pattern()
                self.handle_function_action(action, time_sec)
        elif command == "Pattern":
            strength_str = cmd.get("strength", "")
            rule_str = cmd.get("rule", "")
            self.start_pattern(strength_str, rule_str, time_sec)

    # ==================== TCP-СЕРВЕР ====================
    def handle_client(self, conn):
        conn.settimeout(5)
        try:
            while True:
                data = conn.recv(4096)
                if not data:
                    break
                text = data.decode(errors="ignore").strip()
                for line in text.splitlines():
                    if "{" in line and "}" in line:
                        self.handle_command(line.strip())
                response = {"code": 200, "message": "OK"}
                resp_bytes = json.dumps(response).encode("utf-8")
                conn.sendall(b"HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n" + resp_bytes)
        except Exception:
            pass
        finally:
            conn.close()

    def server_worker(self):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            s.bind((self.HOST, self.PORT))
            s.listen()
            while not self.server_stop_event.is_set():
                try:
                    s.settimeout(1)
                    conn, _ = s.accept()
                    threading.Thread(target=self.handle_client, args=(conn,), daemon=True).start()
                except socket.timeout:
                    continue

    def start_server(self):
        if self.server_thread and self.server_thread.is_alive():
            return
        self._read_config()
        self.server_stop_event.clear()
        self.server_thread = threading.Thread(target=self.server_worker, daemon=True)
        self.server_thread.start()

    def stop_server(self):
        self.server_stop_event.set()
        if self.server_thread and self.server_thread.is_alive():
            self.server_thread.join(timeout=1)
        self.server_thread = None
