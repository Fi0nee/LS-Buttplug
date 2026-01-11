import threading
import time
import keyboard

class KeyboardModule:
    def __init__(self, send_func):
        self.send_func = send_func
        self.active = False
        self.key_bindings = {}  # {'a': 3, 'b': 5}
        self._current_level = 0
        self._lock = threading.Lock()
        self._pressed_keys = set()
        self._thread = threading.Thread(target=self._listener, daemon=True)
        self._thread.start()

    def enable(self):
        self.active = True

    def disable(self):
        self.active = False
        self._set_level(0)
        with self._lock:
            self._pressed_keys.clear()

    def add_binding(self, key: str, level: int):
        key = key.lower()
        if 0 <= level <= 9:
            self.key_bindings[key] = level

    def remove_binding(self, key: str):
        key = key.lower()
        self.key_bindings.pop(key, None)

    def _set_level(self, level: int):
        if level != self._current_level:
            self._current_level = level
            self.send_func(level)

    def _listener(self):
        while True:
            if self.active and self.key_bindings:
                with self._lock:
                    # Обновляем состояние удерживаемых клавиш
                    for key in self.key_bindings:
                        if keyboard.is_pressed(key):
                            self._pressed_keys.add(key)
                        else:
                            self._pressed_keys.discard(key)

                    if self._pressed_keys:
                        # Выбираем максимальный уровень среди удерживаемых клавиш
                        max_level = max(self.key_bindings[k] for k in self._pressed_keys)
                    else:
                        max_level = 0

                self._set_level(max_level)
            time.sleep(0.01)
