import { ESPLoader } from "https://unpkg.com/esptool-js@latest/dist/web/index.js";

const CHIP_CONFIG = {
  esp32: {
    chip: "esp32",
    files: [
      { offset: 0x1000, file: "bootloader-esp32.bin" },
      { offset: 0x8000, file: "partitions-esp32.bin" },
      { offset: 0x10000, file: "firmware-esp32.bin" }
    ]
  },
  esp32c3: {
    chip: "esp32c3",
    files: [
      { offset: 0x0, file: "bootloader-esp32c3.bin" },
      { offset: 0x8000, file: "partitions-esp32c3.bin" },
      { offset: 0x10000, file: "firmware-esp32c3.bin" }
    ]
  },
  esp32s3: {
    chip: "esp32s3",
    files: [
      { offset: 0x0, file: "bootloader-esp32s3.bin" },
      { offset: 0x8000, file: "partitions-esp32s3.bin" },
      { offset: 0x10000, file: "firmware-esp32s3.bin" }
    ]
  }
};

window.connectAndFlash = async function () {
  const board = document.getElementById("boardSelect").value;
  const config = CHIP_CONFIG[board];

  const port = await navigator.serial.requestPort();
  await port.open({ baudRate: 115200 });

  const loader = new ESPLoader({
    transport: port,
    baudrate: 460800,
    terminal: {
      clean() {},
      writeLine(data) { console.log(data); }
    }
  });

  await loader.main();
  await loader.flashId();
  await loader.eraseFlash();

  for (const part of config.files) {
    const res = await fetch(`firmware/${board}/${part.file}`);
    const data = new Uint8Array(await res.arrayBuffer());

    await loader.writeFlash({
      offset: part.offset,
      data,
      erase: false,
      compress: true
    });
  }

  await loader.hardReset();
  alert("Flashing complete");
};
