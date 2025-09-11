[English](README.md) | [Русский](README.ru.md)


# LoveSpouse|Buttplug.io — Управление вибрациями китайских игрушек посредством ESP32 

Проект **LS-Buttplug** предназначен для управления вибрациями дешевых китайских секс-игрушек, которые работают с приложением **MuSe / Love Spouse**.  
Цель проекта — расширить функционал доступных устройств, поскольку большинство дешевых игрушек имеют ограниченное ПО, а приложение **Love Spouse** работает с ограничениями и сбоими в некоторых регионах.

Проект поддерживает 4 режима работы и обеспечивает плавное управление мощностью 0-7, что ограничено прошивкой самих устройств.

---
![Главный интерфейс](img/web.png)
---

## ⚙️ Особенности

- **Ручное плавное управление** через локальный Web-интерфейс  
- **Поддержка игр на ПК** с прямым подключением к Lovense  
- **Совместимость с [Buttplug.io](https://buttplug.io) / [Intiface Central](https://intiface.com/)** как на ПК так и мобильных устройствах
- **Поддержка [XToys.App](https://xtoys.app)** 

---

## 🛠 Установка и настройка

### 1. Подготовка ESP32  
1. Скачайте `.zip` архив с репозитория и распакуйте его на ПК.  
2. Прошивка ESP32:  

**PlatformIO**  
- Откройте папку `PlatformIO` в PlatformIO
- Выберите вашу плату в platformio.ini или добавьте новую

**esptool.py**  
- Установите `esptool`:
  ```bash
  pip install esptool
  ```
- Выберите папку под вашу плату:
   ```bash
   cd ESP/ESP32
   ```
- Выполните прошивку (замените COM1 на ваш порт):
   ```bash
   esptool --chip auto --port COM1 --baud 460800 write_flash -z \
   0x1000 bootloader.bin \
   0x8000 partitions.bin \
   0x10000 firmware.bin
   ```
**У кого нет Python можно прошить запустив Flasher.exe**
- Подключите плату к пк
- Запустите `ESP-Flashing.exe`
- Плата прошьется в атоматическом режиме

### 2. Запуск LS_Buttplug
1. Запустите `LS-Buttplug.exe` или `LS-Buttplug.py`
   - Автоматически откроется Web-страница или перейдите по [http://localhost:5000](http://localhost:5000)  
2. Выберите COM-порт, к которому подключена плата ESP32  
3. Управление:
   - Слайдером мышью или колесиком  
   - 4 режима случайных вибраций
4. Включите **Lovense Game** для подключения игр с прямой поддержкой
    - В настройках игры перейдите в раздел **Lovense**  
    - Укажите HOST: `127.0.0.1`, PORT: `30010` (или другие, совпадающие с Web-интерфейсом)  
    - Снимите галочку "Подключение по SSL" (если есть)  
    - Нажмите "Подключить / Тест подключения"
    - Наслаждайтесь  

### 3. Intiface Central
![Intiface Central](img/IC.png)
- Запускать `LS-Buttplug.py`/`LS-Buttplug.exe` не нужно
- Запустите Intiface Central на ПК или мобильном устройстве и выполните сканирование устройств — устройство будет обнаружено как Lovense  
- Запустите игру или приложение с поддержкой Buttplug.io  
- Наслаждайтесь управлением  

### 4. XToys.App
- Запускать `LS-Buttplug.py`/`LS-Buttplug.exe` не нужно
- Выберите дюбое устройство Lovense Vibrator
- Подключитесь и наслаждайтесь

---

## 🎮 Протестировано в играх:

### Прямое подключение Lovense
- **Helping the Hotties**  
- **Innocent Witches**  

### Intiface Central (ver. 2.6.7)
- **Stardew Valley** — [Buttplug Valley (GitHub)](https://github.com/DryIcedTea/Buttplug-Valley) | [Nexus Mods](https://www.nexusmods.com/stardewvalley/mods/19336)  
- **Terraria** — [Viberaria (GitHub)](https://github.com/notasuka/Viberaria)  
- **Celeste** — [CelestePlug (GameBanana)](https://gamebanana.com/mods/554604)  
- **Minecraft** — [Minegasm (официальный сайт)](https://www.minegasm.net/) | [GitHub](https://github.com/RainbowVille/minegasm)  

> ⚠️ Имеются ошибки в Intiface Central при подключениии устройтва, но они не влияют на работу устройства.
> ⚠️ Возможны небольшие задержки вибраций при работе с Intiface Central.

---

## 🚀 Планы на будущее
- Поддержка двухканальных устройств (вибрация/ротация, вибрация/цвет)
- Прошивка ESP32 через Web-интерфейс
- Файлы .bin прошивки для ESP32s2/3 (Можете прошить/собрать сами используя PlatformIO)
- Расширеные паттерны или запись/воспроизведение
- Привязка вибраций к кнопкам клавиатуры (Например W.A.S.D для разнообразия игрового процесса)
- Вибрация по расписанию


---

## 💡 Поддержка и обратная связь

Если у вас возникли вопросы или предложения, вы можете связаться со мной:
- 📧 Email: [miha.shym@icloud.com](mailto:miha.shym@icloud.com)  
- Обязательно указжите в теме: `LS-Buttplug`

Если вам нравится проект и вы хотите помочь его развитию: 
- ☕BTC: `1KhWiRJhniWBgFffaZkWk7EXuLrK1qjN35`
- ☕USDT (TRC20): `TBG5q6y9f8EE7p8e9naaQKP2UgvjJD5tLT`
- ☕TON: `UQC-FEFn0TojwtGXogMJrnde7TZtJyNZNGa5awliEl03_off`
---

## 📄 Лицензия
MIT License © 2025

